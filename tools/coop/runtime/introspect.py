import os
LOG = os.environ["COOP_OUT"] + ".log"
os.makedirs(os.path.dirname(LOG), exist_ok=True)
def w(m):
    with open(LOG, "a") as f: f.write(str(m) + "\n")
from dolphin import controller
w(f"controller: {[a for a in dir(controller) if not a.startswith('_')]}")
for fn in ("get_gc_buttons","set_gc_buttons"):
    f = getattr(controller, fn, None)
    w(f"{fn} doc: {getattr(f,'__doc__',None)}")
try:
    w(f"get_gc_buttons(0) -> {controller.get_gc_buttons(0)}")
except Exception as e:
    w(f"get_gc_buttons(0) failed: {e!r}")
open(os.environ["COOP_OUT"], "w").write("{}")
w("done")
