import struct, subprocess

data = open("kernel.bin", "rb").read()
print(f"{len(data)} bytes")

(magic,) = struct.unpack("<I", data[:4])
print(hex(magic))
print(
    subprocess.run(["i686-elf-objdump", "-Dhxs", "kernel.bin"],
    capture_output=True, text=True).stdout
)
