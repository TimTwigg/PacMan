import sys
from pathlib import Path

try:
    target = Path(sys.argv[1])
except:
    sys.exit()

if not target.exists():
    print("File not found:", target.name)
    sys.exit()

text = target.read_text()
text = text.replace(";", ",")
text = [l for l in text.strip().split("\n")]
out = []
row = 1
start_coords = None
for l in text:
    col = 0
    new = []
    for c in l:
        new.append(c if (new == [] or new[-1] != c) else "0")
        if new[-1] != ",":
            col += 1
        if c == "," and new[-1] == "0":
            new.append(",")
        if c == "p":
            start_coords = (row, col)
    out.append("".join(new))
    row += 1
out = "\n".join(out).replace("p", "0").replace(",", "") + "\n{x[0]}\n{x[1]}".format(x = start_coords)
with open(target.name[:-4] + "_.csv", "w") as f:
    f.write(out)
print("Successfully translated")
