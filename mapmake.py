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
for l in text:
    new = []
    for c in l:
        new.append(c if (new == [] or new[-1] != c) else "0")
        if c == "," and new[-1] == "0":
            new.append(",")
    out.append("".join(new))
out = "\n".join(out)
with open(target.name, "w") as f:
    f.write(out)
print("Successfully translated")
