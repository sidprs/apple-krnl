import struct, subprocess

data = open("kernel.bin", "rb").read()
print(f"{len(data)} bytes")

(magic,) = struct.unpack("<I", data[:4])

print(subprocess.run(["i686-elf-objdump", "-d", "kernel.bin"],
    capture_output=True, text=True
).stdout)
