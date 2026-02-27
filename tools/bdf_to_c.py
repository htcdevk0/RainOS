# bdf_to_c.py
import sys

font = {}
current = None

with open(sys.argv[1]) as f:
    for line in f:
        line = line.strip()

        if line.startswith("ENCODING"):
            code = int(line.split()[1])
            current = code
            font[current] = []

        elif line == "BITMAP":
            reading = True

        elif line == "ENDCHAR":
            current = None

        elif current is not None and len(line) == 2:
            font[current].append(int(line, 16))

print("const uint8_t font8x16[128][16] = {")
for i in range(128):
    if i in font:
        rows = font[i]
        print(f"    [{i}] = {{", end="")
        for r in rows:
            print(f"0x{r:02X},", end="")
        print("},")
print("};")