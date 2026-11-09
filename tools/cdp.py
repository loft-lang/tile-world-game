# Copyright (c) 2026 Jurjen Stellingwerff
# SPDX-License-Identifier: LGPL-3.0-or-later
# Pictures of the browser page from a headless Chrome (the 3D view, F1, needs a browser's GPU):
#   python3 tools/cdp.py http://localhost:8080/orbury OUTDIR "wait:4000 focus:x key:F1 wait:500 key4:w shot:name"
# A tiny Chrome DevTools client: open a page in headless Chrome, press keys, take screenshots.
# usage: cdp.py URL OUTDIR "script"   script: words separated by spaces —
#   wait:MS   key:NAME (a, w, s, d, F1, F4 ...)   shot:NAME   keyN:NAME (press N times)
import base64, json, os, socket, struct, subprocess, sys, time, urllib.request

CHROME = os.environ.get("CHROME") or next((c for c in ["/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
                                                          "/snap/bin/chromium", "/usr/bin/chromium", "/usr/bin/google-chrome"]
                                                if os.path.exists(c)), "chromium")
url, out, script = sys.argv[1], sys.argv[2], sys.argv[3]
port = 9333
prof = os.path.join(out, "profile")
proc = subprocess.Popen([CHROME, "--headless=new", f"--remote-debugging-port={port}", f"--user-data-dir={prof}",
                         "--enable-unsafe-swiftshader", "--use-angle=swiftshader", "--no-first-run",
                         "--window-size=960,720", "about:blank"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
try:
    for _ in range(50):
        try:
            tabs = json.load(urllib.request.urlopen(f"http://127.0.0.1:{port}/json"))
            break
        except Exception:
            time.sleep(0.2)
    page = [t for t in tabs if t["type"] == "page"][0]
    ws_url = page["webSocketDebuggerUrl"]
    host, path = ws_url[len("ws://"):].split("/", 1)
    h, p = host.split(":")
    s = socket.create_connection((h, int(p)))
    key = base64.b64encode(os.urandom(16)).decode()
    s.send((f"GET /{path} HTTP/1.1\r\nHost: {host}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"
            f"Sec-WebSocket-Key: {key}\r\nSec-WebSocket-Version: 13\r\n\r\n").encode())
    buf = b""
    while b"\r\n\r\n" not in buf:
        buf += s.recv(4096)
    rest = buf.split(b"\r\n\r\n", 1)[1]

    def send(obj):
        data = json.dumps(obj).encode()
        hdr = bytearray([0x81])
        n = len(data)
        if n < 126: hdr.append(0x80 | n)
        elif n < 65536: hdr.append(0x80 | 126); hdr += struct.pack(">H", n)
        else: hdr.append(0x80 | 127); hdr += struct.pack(">Q", n)
        mask = os.urandom(4)
        hdr += mask
        s.send(bytes(hdr) + bytes(b ^ mask[i % 4] for i, b in enumerate(data)))

    def recv_exact(n):
        nonlocal_rest = recv_exact.rest
        while len(nonlocal_rest) < n:
            nonlocal_rest += s.recv(1 << 16)
        recv_exact.rest = nonlocal_rest[n:]
        return nonlocal_rest[:n]
    recv_exact.rest = rest

    def recv():
        msg = b""
        while True:
            b0, b1 = recv_exact(2)
            n = b1 & 0x7f
            if n == 126: n = struct.unpack(">H", recv_exact(2))[0]
            elif n == 127: n = struct.unpack(">Q", recv_exact(8))[0]
            msg += recv_exact(n)
            if b0 & 0x80: return json.loads(msg)

    ids = [0]
    def call(method, **params):
        ids[0] += 1
        send({"id": ids[0], "method": method, "params": params})
        while True:
            m = recv()
            if m.get("id") == ids[0]: return m.get("result", {})

    call("Page.enable")
    call("Page.navigate", url=url)
    codes = {"F1": ("F1", 112), "F4": ("F4", 115), "Enter": ("Enter", 13)}
    def press(name):
        k, c = codes[name] if name in codes else (name, ord(name.upper()))
        txt = name if len(name) == 1 else ""
        call("Input.dispatchKeyEvent", type="keyDown", key=k, code=("Key" + name.upper()) if len(name) == 1 else k,
             windowsVirtualKeyCode=c, text=txt)
        time.sleep(0.09)
        call("Input.dispatchKeyEvent", type="keyUp", key=k, code=("Key" + name.upper()) if len(name) == 1 else k,
             windowsVirtualKeyCode=c)
        time.sleep(0.16)
    for word in script.split():
        cmd, arg = word.split(":", 1)
        if cmd == "wait": time.sleep(int(arg) / 1000)
        elif cmd == "focus": call("Runtime.evaluate", expression="document.querySelector('canvas').focus()")
        elif cmd == "key": press(arg)
        elif cmd.startswith("key"):
            for _ in range(int(cmd[3:])): press(arg)
        elif cmd == "shot":
            r = call("Page.captureScreenshot", format="png")
            open(os.path.join(out, arg + ".png"), "wb").write(base64.b64decode(r["data"]))
            print("shot", arg)
finally:
    proc.terminate()
