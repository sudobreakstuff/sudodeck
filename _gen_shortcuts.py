import json

SC = {}
def app(name, aliases, win, mac=None):
    SC[name] = {"aliases": aliases, "windows": win, "macos": mac or win}

# ── HELPERS ──
K = lambda v: {"type":"key","value":v}
def C(*args):
    if len(args) == 2:
        return {"type":"combo","mod":args[0],"key":args[1]}
    return {"type":"combo","mod":"_".join(args[:-1]),"key":args[-1]}

# ── DISCORD ──
app("discord",["disc","dc","discordapp"],{
    "toggle mute":C("CTRL_SHIFT","m"),"mute":C("CTRL_SHIFT","m"),"unmute":C("CTRL_SHIFT","m"),
    "toggle deafen":C("CTRL_SHIFT","d"),"deafen":C("CTRL_SHIFT","d"),"undeafen":C("CTRL_SHIFT","d"),
    "quick switcher":C("CTRL","k"),"search":C("CTRL","f"),"search all":C("CTRL_SHIFT","f"),
    "mark read":K("ESC"),"mark channel read":C("SHIFT","ESC"),"edit last message":K("UP"),
    "server 1":C("CTRL","1"),"server 2":C("CTRL","2"),"server 3":C("CTRL","3"),
    "server 4":C("CTRL","4"),"server 5":C("CTRL","5"),"server 6":C("CTRL","6"),
    "server 7":C("CTRL","7"),"server 8":C("CTRL","8"),"server 9":C("CTRL","9"),
    "next server":C("CTRL_ALT","DOWN"),"prev server":C("CTRL_ALT","UP"),
    "next channel":C("ALT","DOWN"),"prev channel":C("ALT","UP"),
    "next unread":C("ALT_SHIFT","DOWN"),"prev unread":C("ALT_SHIFT","UP"),
    "answer call":C("CTRL","ENTER"),"reject call":K("ESC"),
    "toggle pins":C("CTRL_SHIFT","p"),"emoji picker":C("CTRL_SHIFT","e"),
    "gif picker":C("CTRL_SHIFT","g"),"sticker picker":C("CTRL_SHIFT","s"),
    "upload file":C("CTRL_SHIFT","u"),"new line":C("SHIFT","ENTER"),"send message":K("ENTER"),
    "bold":C("CTRL","b"),"italic":C("CTRL","i"),"underline":C("CTRL","u"),
    "strikethrough":C("CTRL_SHIFT","x"),"code":C("CTRL_SHIFT","c"),
    "code block":C("CTRL_ALT_SHIFT","c"),"blockquote":C("CTRL_SHIFT","9"),
    "create server":C("CTRL_SHIFT","n"),"help":C("CTRL","/"),"refresh":C("CTRL","r")
},{
    "toggle mute":C("GUI_SHIFT","m"),"mute":C("GUI_SHIFT","m"),"unmute":C("GUI_SHIFT","m"),
    "toggle deafen":C("GUI_SHIFT","d"),"deafen":C("GUI_SHIFT","d"),"undeafen":C("GUI_SHIFT","d"),
    "quick switcher":C("GUI","k"),"search":C("GUI","f"),"search all":C("GUI_SHIFT","f"),
    "mark read":K("ESC"),"mark channel read":C("SHIFT","ESC"),"edit last message":K("UP"),
    "server 1":C("GUI","1"),"server 2":C("GUI","2"),"server 3":C("GUI","3"),
    "server 4":C("GUI","4"),"server 5":C("GUI","5"),"server 6":C("GUI","6"),
    "server 7":C("GUI","7"),"server 8":C("GUI","8"),"server 9":C("GUI","9"),
    "next server":C("GUI_ALT","DOWN"),"prev server":C("GUI_ALT","UP"),
    "next channel":C("ALT","DOWN"),"prev channel":C("ALT","UP"),
    "next unread":C("ALT_SHIFT","DOWN"),"prev unread":C("ALT_SHIFT","UP"),
    "answer call":C("GUI","ENTER"),"reject call":K("ESC"),
    "toggle pins":C("GUI_SHIFT","p"),"emoji picker":C("GUI_SHIFT","e"),
    "gif picker":C("GUI_SHIFT","g"),"sticker picker":C("GUI_SHIFT","s"),
    "upload file":C("GUI_SHIFT","u"),"new line":C("SHIFT","ENTER"),"send message":K("ENTER"),
    "bold":C("GUI","b"),"italic":C("GUI","i"),"underline":C("GUI","u"),
    "strikethrough":C("GUI_SHIFT","x"),"code":C("GUI_SHIFT","c"),
    "code block":C("GUI_ALT_SHIFT","c"),"blockquote":C("GUI_SHIFT","9"),
    "create server":C("GUI_SHIFT","n"),"help":C("GUI","/"),"refresh":C("GUI","r")
})

# ── SLACK ──
app("slack",["slk"],{
    "quick switcher":C("CTRL","k"),"go to channel":C("CTRL","k"),"go to dms":C("CTRL_SHIFT","k"),
    "search":C("CTRL","g"),"search current":C("CTRL","f"),
    "new message":C("CTRL","n"),"mark all read":C("SHIFT","ESC"),
    "toggle mute":K("M"),"toggle video":K("V"),
    "next channel":C("ALT","DOWN"),"prev channel":C("ALT","UP"),
    "next unread":C("ALT_SHIFT","DOWN"),"prev unread":C("ALT_SHIFT","UP"),
    "next section":K("F6"),"prev section":C("SHIFT","F6"),
    "threads":C("CTRL_SHIFT","t"),"activity":C("CTRL_SHIFT","m"),"people":C("CTRL_SHIFT","e"),
    "starred":C("CTRL_SHIFT","s"),"channel info":C("CTRL_SHIFT","i"),
    "toggle right pane":C("CTRL","."),"fullscreen":C("CTRL_SHIFT","f"),
    "edit last message":K("UP"),"upload file":C("CTRL","u"),
    "bold":C("CTRL","b"),"italic":C("CTRL","i"),"strikethrough":C("CTRL_SHIFT","x"),
    "code":C("CTRL_SHIFT","c"),"code block":C("CTRL_ALT_SHIFT","c"),
    "numbered list":C("CTRL_SHIFT","7"),"bulleted list":C("CTRL_SHIFT","8"),
    "blockquote":C("CTRL_SHIFT","9"),"emoji":C("CTRL_SHIFT","\\"),
    "set status":C("CTRL_SHIFT","y"),"preferences":C("CTRL",","),
    "huddle":C("CTRL_SHIFT","h"),"create snippet":C("CTRL_SHIFT","ENTER"),
    "next workspace":C("CTRL","TAB"),"prev workspace":C("CTRL_SHIFT","TAB")
},{
    "quick switcher":C("GUI","k"),"go to channel":C("GUI","k"),"go to dms":C("GUI_SHIFT","k"),
    "search":C("GUI","g"),"search current":C("GUI","f"),
    "new message":C("GUI","n"),"mark all read":C("SHIFT","ESC"),
    "toggle mute":K("M"),"toggle video":K("V"),
    "next channel":C("ALT","DOWN"),"prev channel":C("ALT","UP"),
    "next unread":C("ALT_SHIFT","DOWN"),"prev unread":C("ALT_SHIFT","UP"),
    "next section":K("F6"),"prev section":C("SHIFT","F6"),
    "threads":C("GUI_SHIFT","t"),"activity":C("GUI_SHIFT","m"),"people":C("GUI_SHIFT","e"),
    "starred":C("GUI_SHIFT","s"),"channel info":C("GUI_SHIFT","i"),
    "toggle right pane":C("GUI","."),"fullscreen":C("GUI_SHIFT","f"),
    "edit last message":K("UP"),"upload file":C("GUI","u"),
    "bold":C("GUI","b"),"italic":C("GUI","i"),"strikethrough":C("GUI_SHIFT","x"),
    "code":C("GUI_SHIFT","c"),"code block":C("GUI_ALT_SHIFT","c"),
    "numbered list":C("GUI_SHIFT","7"),"bulleted list":C("GUI_SHIFT","8"),
    "blockquote":C("GUI_SHIFT","9"),"emoji":C("GUI_SHIFT","\\"),
    "set status":C("GUI_SHIFT","y"),"preferences":C("GUI",","),
    "huddle":C("GUI_SHIFT","h"),"create snippet":C("GUI_SHIFT","ENTER"),
    "next workspace":C("GUI","TAB"),"prev workspace":C("GUI_SHIFT","TAB")
})

# ── MICROSOFT TEAMS ──
app("microsoft teams",["teams","msteams","ms teams"],{
    "toggle mute":C("CTRL_SHIFT","m"),"mute":C("CTRL_SHIFT","m"),"unmute":C("CTRL_SHIFT","m"),
    "toggle video":C("CTRL_SHIFT","o"),"video on":C("CTRL_SHIFT","o"),"video off":C("CTRL_SHIFT","o"),
    "share screen":C("CTRL_SHIFT","e"),"raise hand":C("CTRL_SHIFT","k"),"lower hand":C("CTRL_SHIFT","k"),
    "chat":C("CTRL","2"),"activity":C("CTRL","1"),"teams":C("CTRL","3"),
    "calendar":C("CTRL","4"),"calls":C("CTRL","5"),"files":C("CTRL","6"),
    "search":C("CTRL","e"),"go to":C("CTRL","g"),"new chat":C("CTRL","n"),
    "reply":C("CTRL","r"),"attach file":C("CTRL_SHIFT","a"),
    "start meeting":C("CTRL_SHIFT","p"),"join meeting":C("CTRL_SHIFT","j"),
    "end call":C("CTRL_SHIFT","h"),"full screen":C("CTRL_SHIFT","f"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),
    "help":C("CTRL","/"),"bold":C("CTRL","b"),"italic":C("CTRL","i"),"underline":C("CTRL","u")
},{
    "toggle mute":C("GUI_SHIFT","m"),"mute":C("GUI_SHIFT","m"),"unmute":C("GUI_SHIFT","m"),
    "toggle video":C("GUI_SHIFT","o"),"video on":C("GUI_SHIFT","o"),"video off":C("GUI_SHIFT","o"),
    "share screen":C("GUI_SHIFT","e"),"raise hand":C("GUI_SHIFT","k"),"lower hand":C("GUI_SHIFT","k"),
    "chat":C("GUI","2"),"activity":C("GUI","1"),"teams":C("GUI","3"),
    "calendar":C("GUI","4"),"calls":C("GUI","5"),"files":C("GUI","6"),
    "search":C("GUI","e"),"go to":C("GUI","g"),"new chat":C("GUI","n"),
    "reply":C("GUI","r"),"attach file":C("GUI_SHIFT","a"),
    "start meeting":C("GUI_SHIFT","p"),"join meeting":C("GUI_SHIFT","j"),
    "end call":C("GUI_SHIFT","h"),"full screen":C("GUI_SHIFT","f"),
    "zoom in":C("GUI","="),"zoom out":C("GUI","-"),
    "help":C("GUI","/"),"bold":C("GUI","b"),"italic":C("GUI","i"),"underline":C("GUI","u")
})

# ── ZOOM ──
zoom_w = {
    "toggle mute":C("ALT","a"),"mute":C("ALT","a"),"unmute":C("ALT","a"),"push to talk":K("SPACE"),
    "toggle video":C("ALT","v"),"video on":C("ALT","v"),"video off":C("ALT","v"),
    "share screen":C("ALT","s"),"stop share":C("ALT","s"),"pause share":C("ALT","t"),
    "recording":C("ALT","r"),"start recording":C("ALT","r"),"stop recording":C("ALT","r"),
    "participants":C("ALT","u"),"chat":C("ALT","h"),"raise hand":C("ALT","y"),"lower hand":C("ALT","y"),
    "fullscreen":C("ALT","f"),"switch camera":C("ALT","n"),"switch window":C("CTRL","TAB"),
    "leave meeting":C("ALT","q"),"end meeting":C("ALT","q"),
    "whiteboard":C("ALT","w"),"breakout rooms":C("ALT","b"),"invite":C("ALT","i"),
    "mute all":C("CTRL","m"),"unmute all":C("CTRL","u"),
    "zoom in":C("ALT","="),"zoom out":C("ALT","-"),"minimize":K("ESC"),
    "new meeting":C("CTRL","n"),"join meeting":C("CTRL","j")
}
zoom_m = {k: C(v["mod"].replace("CTRL","GUI"), v["key"]) if "CTRL" in v.get("mod","") else v for k,v in zoom_w.items()}
zoom_m["push to talk"] = K("SPACE")
app("zoom",["zoomapp"], zoom_w, zoom_m)

# ── GOOGLE MEET ──
app("google meet",["meet","gmeet","googlemeet"],{
    "toggle mute":C("CTRL","d"),"mute":C("CTRL","d"),"unmute":C("CTRL","d"),
    "toggle video":C("CTRL","e"),"video on":C("CTRL","e"),"video off":C("CTRL","e"),
    "share screen":C("CTRL","s"),"stop share":C("CTRL","s"),
    "chat":C("CTRL","c"),"participants":C("CTRL","p"),"activities":C("CTRL","a"),
    "raise hand":C("CTRL","r"),"lower hand":C("CTRL","r"),"captions":C("CTRL","t"),
    "fullscreen":K("F11"),"show keyboard":C("CTRL","/"),"leave call":C("CTRL","w")
},{
    "toggle mute":C("GUI","d"),"mute":C("GUI","d"),"unmute":C("GUI","d"),
    "toggle video":C("GUI","e"),"video on":C("GUI","e"),"video off":C("GUI","e"),
    "share screen":C("GUI","s"),"stop share":C("GUI","s"),
    "chat":C("GUI","c"),"participants":C("GUI","p"),"activities":C("GUI","a"),
    "raise hand":C("GUI","r"),"lower hand":C("GUI","r"),"captions":C("GUI","t"),
    "fullscreen":K("F11"),"show keyboard":C("GUI","/"),"leave call":C("GUI","w")
})

# ── OBS STUDIO ──
app("obs",["obs studio","obsstudio","streamlabs obs","slobs"],{
    "start streaming":C("CTRL_SHIFT","s"),"stop streaming":C("CTRL_SHIFT","s"),
    "start recording":C("CTRL","r"),"stop recording":C("CTRL","r"),
    "pause recording":C("CTRL_SHIFT","r"),
    "save replay":C("CTRL_SHIFT","b"),"start replay buffer":C("CTRL_SHIFT","m"),"stop replay buffer":C("CTRL_SHIFT","m"),
    "start virtual camera":C("CTRL_SHIFT","t"),"stop virtual camera":C("CTRL_SHIFT","t"),
    "toggle studio mode":C("CTRL_SHIFT","d"),"transition":C("CTRL_SHIFT","t"),"quick transition":C("CTRL_SHIFT","t"),
    "scene 1":C("CTRL","1"),"scene 2":C("CTRL","2"),"scene 3":C("CTRL","3"),
    "scene 4":C("CTRL","4"),"scene 5":C("CTRL","5"),"scene 6":C("CTRL","6"),
    "scene 7":C("CTRL","7"),"scene 8":C("CTRL","8"),"scene 9":C("CTRL","9"),
    "fullscreen":K("F11"),"fullscreen preview":C("CTRL","f"),"lock preview":C("CTRL","l"),
    "settings":C("CTRL","i"),"duplicate source":C("CTRL","d"),
    "copy source":C("CTRL","c"),"paste source":C("CTRL","v"),
    "paste filters":C("CTRL_SHIFT","v"),"paste transform":C("CTRL_ALT","v"),
    "reset transform":K("R"),"fit to screen":K("F"),"stretch to screen":K("S"),"center to screen":K("C"),
    "source up":C("CTRL","UP"),"source down":C("CTRL","DOWN"),
    "toggle mute":C("CTRL","m"),"mute mic":C("CTRL","m"),"unmute mic":C("CTRL","m"),
    "mute desktop":C("CTRL_SHIFT","m"),"unmute desktop":C("CTRL_SHIFT","m"),
    "screenshot":C("CTRL_ALT","s"),"undo":C("CTRL","z"),"redo":C("CTRL","y"),
    "source up top":C("CTRL","HOME"),"source down bottom":C("CTRL","END")
},{
    "start streaming":C("GUI_SHIFT","s"),"stop streaming":C("GUI_SHIFT","s"),
    "start recording":C("GUI","r"),"stop recording":C("GUI","r"),
    "pause recording":C("GUI_SHIFT","r"),
    "save replay":C("GUI_SHIFT","b"),"start replay buffer":C("GUI_SHIFT","m"),"stop replay buffer":C("GUI_SHIFT","m"),
    "start virtual camera":C("GUI_SHIFT","t"),"stop virtual camera":C("GUI_SHIFT","t"),
    "toggle studio mode":C("GUI_SHIFT","d"),"transition":C("GUI_SHIFT","t"),"quick transition":C("GUI_SHIFT","t"),
    "scene 1":C("GUI","1"),"scene 2":C("GUI","2"),"scene 3":C("GUI","3"),
    "scene 4":C("GUI","4"),"scene 5":C("GUI","5"),"scene 6":C("GUI","6"),
    "scene 7":C("GUI","7"),"scene 8":C("GUI","8"),"scene 9":C("GUI","9"),
    "fullscreen":K("F11"),"fullscreen preview":C("GUI","f"),"lock preview":C("GUI","l"),
    "settings":C("GUI","i"),"duplicate source":C("GUI","d"),
    "copy source":C("GUI","c"),"paste source":C("GUI","v"),
    "paste filters":C("GUI_SHIFT","v"),"paste transform":C("GUI_ALT","v"),
    "reset transform":K("R"),"fit to screen":K("F"),"stretch to screen":K("S"),"center to screen":K("C"),
    "source up":C("GUI","UP"),"source down":C("GUI","DOWN"),
    "toggle mute":C("GUI","m"),"mute mic":C("GUI","m"),"unmute mic":C("GUI","m"),
    "mute desktop":C("GUI_SHIFT","m"),"unmute desktop":C("GUI_SHIFT","m"),
    "screenshot":C("GUI_ALT","s"),"undo":C("GUI","z"),"redo":C("GUI","y"),
    "source up top":C("GUI","HOME"),"source down bottom":C("GUI","END")
})

# ── TWITCH ──
app("twitch",["twitchtv"],{
    "search":C("CTRL","f"),"open search":C("CTRL","k"),
    "close":K("ESC"),"fullscreen":K("F"),"mute":K("M"),"unmute":K("M"),
    "volume up":K("UP"),"volume down":K("DOWN"),
    "pause":K("SPACE"),"play":K("SPACE"),
    "toggle theater":K("T"),"chat":K("C"),"emote menu":K("E"),"whisper":K("W"),
    "volume 0":K("0"),"volume 25":K("1"),"volume 50":K("2"),"volume 75":K("3"),"volume 100":K("4")
})

# ── PHOTOSHOP ──
app("photoshop",["ps","photoshop cc","adobe photoshop"],{
    "move tool":K("V"),"rectangular marquee":K("M"),"lasso":K("L"),"magic wand":K("W"),
    "crop":K("C"),"eyedropper":K("I"),"eraser":K("E"),"gradient":K("G"),
    "brush":K("B"),"healing brush":K("J"),"clone stamp":K("S"),
    "pen":K("P"),"type tool":K("T"),"hand tool":K("H"),"zoom tool":K("Z"),
    "select all":C("CTRL","a"),"deselect":C("CTRL","d"),"inverse selection":C("CTRL_SHIFT","i"),
    "free transform":C("CTRL","t"),
    "new layer":C("CTRL_SHIFT","n"),"layer via copy":C("CTRL","j"),
    "group layers":C("CTRL","g"),"ungroup":C("CTRL_SHIFT","g"),
    "merge layers":C("CTRL","e"),"merge visible":C("CTRL_SHIFT","e"),
    "level up":C("CTRL","]"),"level down":C("CTRL","["),
    "levels":C("CTRL","l"),"curves":C("CTRL","m"),"hue saturation":C("CTRL","u"),
    "color balance":C("CTRL","b"),"desaturate":C("CTRL_SHIFT","u"),"invert":C("CTRL","i"),
    "last filter":C("CTRL","f"),"fill":C("SHIFT","F5"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),"save for web":C("CTRL_ALT_SHIFT","s"),
    "undo":C("CTRL","z"),"step back":C("CTRL_ALT","z"),"step forward":C("CTRL_SHIFT","z"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),"fit to screen":C("CTRL","0"),"actual pixels":C("CTRL","1"),
    "show rulers":C("CTRL","r"),"show grid":C("CTRL","'"),
    "new document":C("CTRL","n"),"open":C("CTRL","o")
},{
    "move tool":K("V"),"rectangular marquee":K("M"),"lasso":K("L"),"magic wand":K("W"),
    "crop":K("C"),"eyedropper":K("I"),"eraser":K("E"),"gradient":K("G"),
    "brush":K("B"),"healing brush":K("J"),"clone stamp":K("S"),
    "pen":K("P"),"type tool":K("T"),"hand tool":K("H"),"zoom tool":K("Z"),
    "select all":C("GUI","a"),"deselect":C("GUI","d"),"inverse selection":C("GUI_SHIFT","i"),
    "free transform":C("GUI","t"),
    "new layer":C("GUI_SHIFT","n"),"layer via copy":C("GUI","j"),
    "group layers":C("GUI","g"),"ungroup":C("GUI_SHIFT","g"),
    "merge layers":C("GUI","e"),"merge visible":C("GUI_SHIFT","e"),
    "level up":C("GUI","]"),"level down":C("GUI","["),
    "levels":C("GUI","l"),"curves":C("GUI","m"),"hue saturation":C("GUI","u"),
    "color balance":C("GUI","b"),"desaturate":C("GUI_SHIFT","u"),"invert":C("GUI","i"),
    "last filter":C("GUI","f"),"fill":C("SHIFT","F5"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),"save for web":C("GUI_ALT_SHIFT","s"),
    "undo":C("GUI","z"),"step back":C("GUI_ALT","z"),"step forward":C("GUI_SHIFT","z"),
    "zoom in":C("GUI","="),"zoom out":C("GUI","-"),"fit to screen":C("GUI","0"),"actual pixels":C("GUI","1"),
    "show rulers":C("GUI","r"),"show grid":C("GUI","'"),
    "new document":C("GUI","n"),"open":C("GUI","o")
})

# ── PREMIERE PRO ──
app("premiere pro",["premiere","adobe premiere","pr","adobe premiere pro"],{
    "play":K("SPACE"),"pause":K("SPACE"),"play reverse":K("J"),"stop":K("K"),"play forward":K("L"),
    "slow reverse":C("SHIFT","J"),"slow forward":C("SHIFT","L"),
    "frame back":K("LEFT"),"frame forward":K("RIGHT"),
    "cut":C("CTRL","k"),"ripple delete":C("SHIFT","DELETE"),
    "mark in":K("I"),"mark out":K("O"),
    "go to in":C("CTRL_SHIFT","i"),"go to out":C("CTRL_SHIFT","o"),
    "add marker":K("M"),"go to next marker":C("CTRL_SHIFT","RIGHT"),"go to prev marker":C("CTRL_SHIFT","LEFT"),
    "snap":K("S"),"add edit":C("CTRL","k"),"add edit all tracks":C("CTRL_SHIFT","k"),
    "match frame":K("F"),
    "new project":C("CTRL_ALT","n"),"new sequence":C("CTRL","n"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),
    "export media":C("CTRL","m"),"import":C("CTRL","i"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "duplicate":C("CTRL_SHIFT","/"),"select all":C("CTRL","a"),"deselect all":C("CTRL_SHIFT","a"),
    "find":C("CTRL","f"),"group":C("CTRL","g"),"ungroup":C("CTRL_SHIFT","g"),
    "link":C("CTRL","l"),"enable clip":C("SHIFT","e"),"speed duration":C("CTRL","r"),
    "default transition":C("CTRL","d"),"render in to out":K("ENTER"),
    "fullscreen":C("CTRL","~"),"zoom in":K("="),"zoom out":K("-"),
    "selection tool":K("V"),"razor tool":K("C"),"slip tool":K("Y"),"slide tool":K("U"),
    "rate stretch tool":K("R"),"pen tool":K("P"),"hand tool":K("H")
},{
    "play":K("SPACE"),"pause":K("SPACE"),"play reverse":K("J"),"stop":K("K"),"play forward":K("L"),
    "slow reverse":C("SHIFT","J"),"slow forward":C("SHIFT","L"),
    "frame back":K("LEFT"),"frame forward":K("RIGHT"),
    "cut":C("GUI","k"),"ripple delete":C("SHIFT","DELETE"),
    "mark in":K("I"),"mark out":K("O"),
    "go to in":C("GUI_SHIFT","i"),"go to out":C("GUI_SHIFT","o"),
    "add marker":K("M"),"go to next marker":C("GUI_SHIFT","RIGHT"),"go to prev marker":C("GUI_SHIFT","LEFT"),
    "snap":K("S"),"add edit":C("GUI","k"),"add edit all tracks":C("GUI_SHIFT","k"),
    "match frame":K("F"),
    "new project":C("GUI_ALT","n"),"new sequence":C("GUI","n"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "export media":C("GUI","m"),"import":C("GUI","i"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "duplicate":C("GUI_SHIFT","/"),"select all":C("GUI","a"),"deselect all":C("GUI_SHIFT","a"),
    "find":C("GUI","f"),"group":C("GUI","g"),"ungroup":C("GUI_SHIFT","g"),
    "link":C("GUI","l"),"enable clip":C("SHIFT","e"),"speed duration":C("GUI","r"),
    "default transition":C("GUI","d"),"render in to out":K("ENTER"),
    "fullscreen":C("GUI","~"),
    "selection tool":K("V"),"razor tool":K("C"),"slip tool":K("Y"),"slide tool":K("U"),
    "rate stretch tool":K("R"),"pen tool":K("P"),"hand tool":K("H")
})

# ── AFTER EFFECTS ──
app("after effects",["ae","adobe after effects","aftereffects"],{
    "play":K("SPACE"),"pause":K("SPACE"),"play reverse":C("CTRL","SPACE"),
    "frame back":K("PAGE_UP"),"frame forward":K("PAGE_DOWN"),"go to time":C("CTRL","g"),
    "mark in":K("B"),"mark out":K("N"),"ram preview":K("0"),
    "save":C("CTRL","s"),"undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "new comp":C("CTRL","n"),"new folder":C("CTRL_ALT_SHIFT","n"),
    "import":C("CTRL","i"),"import multiple":C("CTRL_ALT","i"),
    "cut":C("CTRL","x"),"copy":C("CTRL","c"),"paste":C("CTRL","v"),"duplicate":C("CTRL","d"),
    "split layer":C("CTRL_SHIFT","d"),"precompose":C("CTRL_SHIFT","c"),"toggle nulls":C("CTRL_SHIFT","p"),
    "position":K("P"),"scale":K("S"),"rotation":K("R"),"opacity":K("T"),"anchor point":K("A"),
    "show audio":K("L"),"show expression":K("E"),"show masks":K("M"),"show effects":K("E"),
    "trim to comp":C("CTRL_ALT","x"),"fit to comp":C("CTRL_ALT","f"),"center in view":C("CTRL_ALT","HOME"),
    "fullscreen":C("CTRL","~"),"zoom in":K("."),"zoom out":K(","),
    "selection tool":K("V"),"hand tool":K("H"),"zoom tool":K("Z"),"rotation tool":K("W"),
    "camera tool":K("C"),"pan behind tool":K("Y"),"rectangle tool":K("Q"),"pen tool":K("G")
},{
    "play":K("SPACE"),"pause":K("SPACE"),"play reverse":C("GUI","SPACE"),
    "frame back":K("PAGE_UP"),"frame forward":K("PAGE_DOWN"),"go to time":C("GUI","g"),
    "mark in":K("B"),"mark out":K("N"),"ram preview":K("0"),
    "save":C("GUI","s"),"undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "new comp":C("GUI","n"),"new folder":C("GUI_ALT_SHIFT","n"),
    "import":C("GUI","i"),"import multiple":C("GUI_ALT","i"),
    "cut":C("GUI","x"),"copy":C("GUI","c"),"paste":C("GUI","v"),"duplicate":C("GUI","d"),
    "split layer":C("GUI_SHIFT","d"),"precompose":C("GUI_SHIFT","c"),"toggle nulls":C("GUI_SHIFT","p"),
    "position":K("P"),"scale":K("S"),"rotation":K("R"),"opacity":K("T"),"anchor point":K("A"),
    "show audio":K("L"),"show expression":K("E"),"show masks":K("M"),"show effects":K("E"),
    "trim to comp":C("GUI_ALT","x"),"fit to comp":C("GUI_ALT","f"),"center in view":C("GUI_ALT","HOME"),
    "fullscreen":C("GUI","~"),
    "selection tool":K("V"),"hand tool":K("H"),"zoom tool":K("Z"),"rotation tool":K("W"),
    "camera tool":K("C"),"pan behind tool":K("Y"),"rectangle tool":K("Q"),"pen tool":K("G")
})

# ── ILLUSTRATOR ──
app("illustrator",["ai","adobe illustrator"],{
    "selection tool":K("V"),"direct selection":K("A"),"pen tool":K("P"),"type tool":K("T"),
    "line tool":K("\\"),"rectangle tool":K("M"),"ellipse tool":K("L"),
    "paintbrush":K("B"),"pencil":K("N"),"blob brush":C("SHIFT","B"),
    "eraser":C("SHIFT","E"),"scissors":K("C"),"rotate tool":K("R"),"scale tool":K("S"),
    "warp tool":C("SHIFT","R"),"free transform":K("E"),
    "shape builder":C("SHIFT","M"),"live paint":K("K"),"gradient tool":K("G"),"eyedropper":K("I"),
    "hand tool":K("H"),"zoom tool":K("Z"),
    "group":C("CTRL","g"),"ungroup":C("CTRL_SHIFT","g"),
    "lock selection":C("CTRL","2"),"unlock all":C("CTRL_ALT","2"),
    "hide selection":C("CTRL","3"),"show all":C("CTRL_ALT","3"),
    "send backward":C("CTRL","["),"send to back":C("CTRL_SHIFT","["),
    "bring forward":C("CTRL","]"),"bring to front":C("CTRL_SHIFT","]"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),"export":C("CTRL_ALT_SHIFT","w"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "select all":C("CTRL","a"),"deselect":C("CTRL_SHIFT","a"),"inverse selection":C("CTRL_SHIFT","i"),
    "paste in front":C("CTRL","f"),"paste in back":C("CTRL","b"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),"fit artboard":C("CTRL","0"),"actual size":C("CTRL","1"),
    "new document":C("CTRL","n"),"open":C("CTRL","o"),"fullscreen":K("F")
},{
    "selection tool":K("V"),"direct selection":K("A"),"pen tool":K("P"),"type tool":K("T"),
    "line tool":K("\\"),"rectangle tool":K("M"),"ellipse tool":K("L"),
    "paintbrush":K("B"),"pencil":K("N"),"blob brush":C("SHIFT","B"),
    "eraser":C("SHIFT","E"),"scissors":K("C"),"rotate tool":K("R"),"scale tool":K("S"),
    "warp tool":C("SHIFT","R"),"free transform":K("E"),
    "shape builder":C("SHIFT","M"),"live paint":K("K"),"gradient tool":K("G"),"eyedropper":K("I"),
    "hand tool":K("H"),"zoom tool":K("Z"),
    "group":C("GUI","g"),"ungroup":C("GUI_SHIFT","g"),
    "lock selection":C("GUI","2"),"unlock all":C("GUI_ALT","2"),
    "hide selection":C("GUI","3"),"show all":C("GUI_ALT","3"),
    "send backward":C("GUI","["),"send to back":C("GUI_SHIFT","["),
    "bring forward":C("GUI","]"),"bring to front":C("GUI_SHIFT","]"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),"export":C("GUI_ALT_SHIFT","w"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "select all":C("GUI","a"),"deselect":C("GUI_SHIFT","a"),"inverse selection":C("GUI_SHIFT","i"),
    "paste in front":C("GUI","f"),"paste in back":C("GUI","b"),
    "zoom in":C("GUI","="),"zoom out":C("GUI","-"),"fit artboard":C("GUI","0"),"actual size":C("GUI","1"),
    "new document":C("GUI","n"),"open":C("GUI","o"),"fullscreen":K("F")
})

# ── DAVINCI RESOLVE ──
app("davinci resolve",["davinci","resolve","dr","blackmagic resolve"],{
    "play":K("SPACE"),"pause":K("SPACE"),"play reverse":K("J"),"stop":K("K"),"play forward":K("L"),
    "frame back":K("LEFT"),"frame forward":K("RIGHT"),
    "mark in":K("I"),"mark out":K("O"),"mark clip":K("X"),
    "cut":C("CTRL","b"),"ripple delete":C("CTRL_SHIFT","x"),"add edit":C("CTRL","\\"),
    "overwrite":K("F10"),"insert":K("F9"),"replace":K("F11"),"place on top":K("F12"),
    "snap":K("N"),"ripple trim":K("W"),"roll edit":K("R"),"slip edit":K("Y"),"slide edit":K("U"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),
    "import media":C("CTRL","i"),"new project":C("CTRL","n"),
    "fullscreen":C("CTRL","f"),"fit timeline":C("CTRL_SHIFT","f"),
    "media page":C("CTRL_SHIFT","1"),"cut page":C("CTRL_SHIFT","2"),
    "edit page":C("CTRL_SHIFT","3"),"color page":C("CTRL_SHIFT","4"),
    "deliver page":C("CTRL_SHIFT","5"),"fairlight page":C("CTRL_SHIFT","6"),
    "previous edit":K("UP"),"next edit":K("DOWN"),
    "link clips":C("CTRL_SHIFT","l"),"group clips":C("CTRL","g"),"ungroup":C("CTRL_SHIFT","g"),
    "copy":C("CTRL","c"),"paste":C("CTRL","v"),"cut":C("CTRL","x"),
    "select all":C("CTRL","a"),"deselect all":C("CTRL_SHIFT","a")
},{
    "play":K("SPACE"),"pause":K("SPACE"),"play reverse":K("J"),"stop":K("K"),"play forward":K("L"),
    "frame back":K("LEFT"),"frame forward":K("RIGHT"),
    "mark in":K("I"),"mark out":K("O"),"mark clip":K("X"),
    "cut":C("GUI","b"),"ripple delete":C("GUI_SHIFT","x"),"add edit":C("GUI","\\"),
    "overwrite":K("F10"),"insert":K("F9"),"replace":K("F11"),"place on top":K("F12"),
    "snap":K("N"),"ripple trim":K("W"),"roll edit":K("R"),"slip edit":K("Y"),"slide edit":K("U"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "import media":C("GUI","i"),"new project":C("GUI","n"),
    "fullscreen":C("GUI","f"),"fit timeline":C("GUI_SHIFT","f"),
    "media page":C("GUI_SHIFT","1"),"cut page":C("GUI_SHIFT","2"),
    "edit page":C("GUI_SHIFT","3"),"color page":C("GUI_SHIFT","4"),
    "deliver page":C("GUI_SHIFT","5"),"fairlight page":C("GUI_SHIFT","6"),
    "previous edit":K("UP"),"next edit":K("DOWN"),
    "link clips":C("GUI_SHIFT","l"),"group clips":C("GUI","g"),"ungroup":C("GUI_SHIFT","g"),
    "copy":C("GUI","c"),"paste":C("GUI","v"),"cut":C("GUI","x"),
    "select all":C("GUI","a"),"deselect all":C("GUI_SHIFT","a")
})

# ── SPOTIFY ──
app("spotify",["spot"],{
    "play":K("MEDIA_PLAY_PAUSE"),"pause":K("MEDIA_PLAY_PAUSE"),"play pause":K("MEDIA_PLAY_PAUSE"),
    "next":K("MEDIA_NEXT_TRACK"),"next track":K("MEDIA_NEXT_TRACK"),
    "previous":K("MEDIA_PREV_TRACK"),"prev":K("MEDIA_PREV_TRACK"),"previous track":K("MEDIA_PREV_TRACK"),
    "volume up":K("MEDIA_VOLUME_UP"),"volume down":K("MEDIA_VOLUME_DOWN"),
    "mute":K("MEDIA_MUTE"),"unmute":K("MEDIA_MUTE"),
    "shuffle":C("CTRL","s"),"repeat":C("CTRL","r"),
    "search":C("CTRL","k"),"like":C("CTRL","l"),"save":C("CTRL","l"),
    "queue":C("CTRL_SHIFT","q"),"new playlist":C("CTRL","n"),
    "home":C("CTRL","1"),"library":C("CTRL","2"),"now playing":C("CTRL","3"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),"fullscreen":K("F11")
},{
    "play":K("MEDIA_PLAY_PAUSE"),"pause":K("MEDIA_PLAY_PAUSE"),"play pause":K("MEDIA_PLAY_PAUSE"),
    "next":K("MEDIA_NEXT_TRACK"),"next track":K("MEDIA_NEXT_TRACK"),
    "previous":K("MEDIA_PREV_TRACK"),"prev":K("MEDIA_PREV_TRACK"),"previous track":K("MEDIA_PREV_TRACK"),
    "volume up":K("MEDIA_VOLUME_UP"),"volume down":K("MEDIA_VOLUME_DOWN"),
    "mute":K("MEDIA_MUTE"),"unmute":K("MEDIA_MUTE"),
    "shuffle":C("GUI","s"),"repeat":C("GUI","r"),
    "search":C("GUI","k"),"like":C("GUI","l"),"save":C("GUI","l"),
    "queue":C("GUI_SHIFT","q"),"new playlist":C("GUI","n"),
    "home":C("GUI","1"),"library":C("GUI","2"),"now playing":C("GUI","3"),
    "fullscreen":K("F11")
})

# ── YOUTUBE MUSIC ──
app("youtube music",["ytm","yt music"],{
    "play":K("MEDIA_PLAY_PAUSE"),"pause":K("MEDIA_PLAY_PAUSE"),"play pause":K("MEDIA_PLAY_PAUSE"),
    "next":K("MEDIA_NEXT_TRACK"),"next track":K("MEDIA_NEXT_TRACK"),
    "previous":K("MEDIA_PREV_TRACK"),"prev":K("MEDIA_PREV_TRACK"),"previous track":K("MEDIA_PREV_TRACK"),
    "volume up":K("MEDIA_VOLUME_UP"),"volume down":K("MEDIA_VOLUME_DOWN"),
    "mute":K("MEDIA_MUTE"),"unmute":K("MEDIA_MUTE"),
    "search":{"type":"key","value":"/"},"like":C("CTRL","l"),"dislike":C("CTRL","d")
},{
    "play":K("MEDIA_PLAY_PAUSE"),"pause":K("MEDIA_PLAY_PAUSE"),"play pause":K("MEDIA_PLAY_PAUSE"),
    "next":K("MEDIA_NEXT_TRACK"),"next track":K("MEDIA_NEXT_TRACK"),
    "previous":K("MEDIA_PREV_TRACK"),"prev":K("MEDIA_PREV_TRACK"),"previous track":K("MEDIA_PREV_TRACK"),
    "volume up":K("MEDIA_VOLUME_UP"),"volume down":K("MEDIA_VOLUME_DOWN"),
    "mute":K("MEDIA_MUTE"),"unmute":K("MEDIA_MUTE"),
    "search":{"type":"key","value":"/"},"like":C("GUI","l"),"dislike":C("GUI","d")
})

# ── ABLETON LIVE ──
app("ableton live",["ableton","live","abletonlive"],{
    "play":K("SPACE"),"pause":K("SPACE"),"stop":C("CTRL","SPACE"),
    "record":K("F9"),"start recording":K("F9"),"stop recording":K("F9"),
    "metronome":C("CTRL","m"),"toggle metronome":C("CTRL","m"),
    "toggle views":K("TAB"),"toggle detail":C("SHIFT","TAB"),
    "browser":C("CTRL_ALT","b"),"toggle browser":C("CTRL_ALT","b"),
    "quantize":C("CTRL","u"),"duplicate":C("CTRL","d"),
    "delete":K("DELETE"),"undo":C("CTRL","z"),"redo":C("CTRL","y"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),
    "new live set":C("CTRL","n"),"open":C("CTRL","o"),
    "select all":C("CTRL","a"),"cut":C("CTRL","x"),"copy":C("CTRL","c"),"paste":C("CTRL","v"),
    "rename":C("CTRL","r"),"fullscreen":K("F11"),
    "midi map":C("CTRL","m"),"key map":C("CTRL","k"),"preferences":C("CTRL",","),
    "solo":K("S"),"mute":K("M"),"arm":K("R"),
    "stop clip":K("F10"),"launch clip":K("F11"),"overdub":K("F12"),
    "nudge left":C("CTRL","LEFT"),"nudge right":C("CTRL","RIGHT"),
    "tap tempo":K("T"),"follow":C("CTRL","f")
},{
    "play":K("SPACE"),"pause":K("SPACE"),"stop":C("GUI","SPACE"),
    "record":K("F9"),"start recording":K("F9"),"stop recording":K("F9"),
    "metronome":C("GUI","m"),"toggle metronome":C("GUI","m"),
    "toggle views":K("TAB"),"toggle detail":C("SHIFT","TAB"),
    "browser":C("GUI_ALT","b"),"toggle browser":C("GUI_ALT","b"),
    "quantize":C("GUI","u"),"duplicate":C("GUI","d"),
    "delete":K("DELETE"),"undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "new live set":C("GUI","n"),"open":C("GUI","o"),
    "select all":C("GUI","a"),"cut":C("GUI","x"),"copy":C("GUI","c"),"paste":C("GUI","v"),
    "rename":C("GUI","r"),"fullscreen":K("F11"),
    "midi map":C("GUI","m"),"key map":C("GUI","k"),"preferences":C("GUI",","),
    "solo":K("S"),"mute":K("M"),"arm":K("R"),
    "stop clip":K("F10"),"launch clip":K("F11"),"overdub":K("F12"),
    "nudge left":C("GUI","LEFT"),"nudge right":C("GUI","RIGHT"),
    "tap tempo":K("T"),"follow":C("GUI","f")
})

# ── FL STUDIO ──
app("fl studio",["fl","fruity loops"],{
    "play":K("SPACE"),"pause":K("SPACE"),"play pause":K("SPACE"),
    "record":K("R"),"start recording":K("R"),"stop recording":K("R"),
    "toggle pattern song":K("L"),
    "piano roll":K("F7"),"playlist":K("F5"),"channel rack":K("F6"),"mixer":K("F9"),
    "browser":K("F8"),"step sequencer":K("F6"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "cut":C("CTRL","x"),"copy":C("CTRL","c"),"paste":C("CTRL","v"),"duplicate":C("CTRL","b"),
    "delete":K("DELETE"),"select all":C("CTRL","a"),"deselect":C("CTRL","d"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),
    "new":C("CTRL","n"),"open":C("CTRL","o"),
    "quantize":C("CTRL","q"),"solo":K("S"),"mute":K("M"),"rename":K("F2"),
    "add marker":K("I"),"toggle snap":K("J"),
    "metronome":C("CTRL","m"),"panic":C("CTRL","h"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),
    "pencil tool":K("P"),"paint tool":K("B"),"select tool":K("E"),"slice tool":K("S"),"mute tool":K("M"),
    "preferences":K("F10"),"render":C("CTRL","r"),"export":C("CTRL_SHIFT","r")
},{
    "play":K("SPACE"),"pause":K("SPACE"),"play pause":K("SPACE"),
    "record":K("R"),"start recording":K("R"),"stop recording":K("R"),
    "toggle pattern song":K("L"),
    "piano roll":K("F7"),"playlist":K("F5"),"channel rack":K("F6"),"mixer":K("F9"),
    "browser":K("F8"),"step sequencer":K("F6"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "cut":C("GUI","x"),"copy":C("GUI","c"),"paste":C("GUI","v"),"duplicate":C("GUI","b"),
    "delete":K("DELETE"),"select all":C("GUI","a"),"deselect":C("GUI","d"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "new":C("GUI","n"),"open":C("GUI","o"),
    "quantize":C("GUI","q"),"solo":K("S"),"mute":K("M"),"rename":K("F2"),
    "add marker":K("I"),"toggle snap":K("J"),
    "metronome":C("GUI","m"),"panic":C("GUI","h"),
    "pencil tool":K("P"),"paint tool":K("B"),"select tool":K("E"),"slice tool":K("S"),"mute tool":K("M"),
    "preferences":K("F10"),"render":C("GUI","r"),"export":C("GUI_SHIFT","r")
})

# ── AUDACITY ──
app("audacity",["auda"],{
    "play":K("SPACE"),"pause":K("SPACE"),"stop":K("SPACE"),"play stop":K("SPACE"),
    "record":K("R"),"start recording":K("R"),"stop recording":K("R"),
    "cut":C("CTRL","x"),"copy":C("CTRL","c"),"paste":C("CTRL","v"),
    "trim":C("CTRL","t"),"silence":C("CTRL","l"),"split":C("CTRL","i"),"split new":C("CTRL_SHIFT","i"),
    "select all":C("CTRL","a"),"deselect":C("CTRL_SHIFT","a"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "save":C("CTRL","s"),"export":C("CTRL_SHIFT","e"),"export mp3":C("CTRL_ALT_SHIFT","e"),
    "import audio":C("CTRL_SHIFT","i"),
    "zoom in":C("CTRL","1"),"zoom out":C("CTRL","2"),"zoom to selection":C("CTRL","3"),"fit in window":C("CTRL","0"),
    "fullscreen":K("F11"),"mute":C("CTRL","u"),"unmute":C("CTRL","u"),"toggle mute":C("CTRL","u"),
    "solo":C("CTRL","s")
},{
    "play":K("SPACE"),"pause":K("SPACE"),"stop":K("SPACE"),"play stop":K("SPACE"),
    "record":K("R"),"start recording":K("R"),"stop recording":K("R"),
    "cut":C("GUI","x"),"copy":C("GUI","c"),"paste":C("GUI","v"),
    "trim":C("GUI","t"),"silence":C("GUI","l"),"split":C("GUI","i"),"split new":C("GUI_SHIFT","i"),
    "select all":C("GUI","a"),"deselect":C("GUI_SHIFT","a"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "save":C("GUI","s"),"export":C("GUI_SHIFT","e"),"export mp3":C("GUI_ALT_SHIFT","e"),
    "import audio":C("GUI_SHIFT","i"),
    "zoom in":C("GUI","1"),"zoom out":C("GUI","2"),"zoom to selection":C("GUI","3"),"fit in window":C("GUI","0"),
    "fullscreen":K("F11"),"mute":C("GUI","u"),"unmute":C("GUI","u"),"toggle mute":C("GUI","u"),
    "solo":C("GUI","s")
})

# ── CHROME ──
chrome_win = {
    "new tab":C("CTRL","t"),"close tab":C("CTRL","w"),"reopen tab":C("CTRL_SHIFT","t"),
    "next tab":C("CTRL","TAB"),"prev tab":C("CTRL_SHIFT","TAB"),
    "tab 1":C("CTRL","1"),"tab 2":C("CTRL","2"),"tab 3":C("CTRL","3"),
    "tab 4":C("CTRL","4"),"tab 5":C("CTRL","5"),"tab 6":C("CTRL","6"),
    "tab 7":C("CTRL","7"),"tab 8":C("CTRL","8"),"tab last":C("CTRL","9"),
    "new window":C("CTRL","n"),"new incognito":C("CTRL_SHIFT","n"),
    "search":C("CTRL","k"),"address bar":C("CTRL","l"),"go to url":C("CTRL","l"),
    "history":C("CTRL","h"),"bookmarks":C("CTRL_SHIFT","b"),
    "bookmark this":C("CTRL","d"),"bookmark all":C("CTRL_SHIFT","d"),
    "find":C("CTRL","f"),"find next":C("CTRL","g"),"find prev":C("CTRL_SHIFT","g"),
    "reload":K("F5"),"hard reload":C("CTRL_SHIFT","r"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),"zoom reset":C("CTRL","0"),
    "fullscreen":K("F11"),"devtools":K("F12"),
    "console":C("CTRL_SHIFT","j"),"source":C("CTRL","u"),
    "downloads":C("CTRL","j"),"clear browsing":C("CTRL_SHIFT","DELETE"),
    "back":{"type":"key","value":"ALT_LEFT"},"forward":{"type":"key","value":"ALT_RIGHT"},
    "print":C("CTRL","p"),"save page":C("CTRL","s"),
    "mute tab":C("CTRL","m"),"task manager":C("SHIFT","ESC")
}
chrome_mac = {k: C(v["mod"].replace("CTRL","GUI"), v["key"]) if "combo" in v.get("type","") else v for k,v in chrome_win.items()}
chrome_mac["back"] = C("GUI","[")
chrome_mac["forward"] = C("GUI","]")
chrome_mac["task manager"] = C("SHIFT","ESC")
app("chrome",["google chrome","browser"], chrome_win, chrome_mac)

# ── FIREFOX ──
app("firefox",["ff","mozilla firefox"],{
    "new tab":C("CTRL","t"),"close tab":C("CTRL","w"),"reopen tab":C("CTRL_SHIFT","t"),
    "next tab":C("CTRL","TAB"),"prev tab":C("CTRL_SHIFT","TAB"),
    "tab 1":C("CTRL","1"),"tab 2":C("CTRL","2"),"tab 3":C("CTRL","3"),
    "tab 4":C("CTRL","4"),"tab 5":C("CTRL","5"),"tab 6":C("CTRL","6"),
    "tab 7":C("CTRL","7"),"tab 8":C("CTRL","8"),"tab last":C("CTRL","9"),
    "new window":C("CTRL","n"),"new private":C("CTRL_SHIFT","n"),
    "search":C("CTRL","k"),"address bar":C("CTRL","l"),"go to url":C("CTRL","l"),
    "history":C("CTRL","h"),"bookmarks":C("CTRL_SHIFT","b"),
    "bookmark this":C("CTRL","d"),"find":C("CTRL","f"),"find next":K("F3"),
    "reload":K("F5"),"hard reload":C("CTRL_SHIFT","r"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),"zoom reset":C("CTRL","0"),
    "fullscreen":K("F11"),"devtools":C("CTRL_SHIFT","i"),
    "console":C("CTRL_SHIFT","k"),"source":C("CTRL","u"),
    "downloads":C("CTRL","j"),"print":C("CTRL","p"),
    "back":{"type":"key","value":"ALT_LEFT"},"forward":{"type":"key","value":"ALT_RIGHT"},
    "mute tab":C("CTRL","m"),"clear browsing":C("CTRL_SHIFT","DELETE"),
    "reader mode":K("F9")
},{
    "new tab":C("GUI","t"),"close tab":C("GUI","w"),"reopen tab":C("GUI_SHIFT","t"),
    "next tab":C("GUI","TAB"),"prev tab":C("GUI_SHIFT","TAB"),
    "tab 1":C("GUI","1"),"tab 2":C("GUI","2"),"tab 3":C("GUI","3"),
    "tab 4":C("GUI","4"),"tab 5":C("GUI","5"),"tab 6":C("GUI","6"),
    "tab 7":C("GUI","7"),"tab 8":C("GUI","8"),"tab last":C("GUI","9"),
    "new window":C("GUI","n"),"new private":C("GUI_SHIFT","n"),
    "search":C("GUI","k"),"address bar":C("GUI","l"),"go to url":C("GUI","l"),
    "history":C("GUI","h"),"bookmarks":C("GUI_SHIFT","b"),
    "bookmark this":C("GUI","d"),"find":C("GUI","f"),"find next":K("F3"),
    "reload":K("F5"),"hard reload":C("GUI_SHIFT","r"),
    "zoom in":C("GUI","="),"zoom out":C("GUI","-"),"zoom reset":C("GUI","0"),
    "fullscreen":K("F11"),"devtools":C("GUI_SHIFT","i"),
    "console":C("GUI_SHIFT","k"),"source":C("GUI","u"),
    "downloads":C("GUI","j"),"print":C("GUI","p"),
    "back":C("GUI","["),"forward":C("GUI","]"),
    "mute tab":C("GUI","m"),"clear browsing":C("GUI_SHIFT","DELETE"),
    "reader mode":K("F9")
})

# ── VSCODE ──
app("vscode",["vs code","visual studio code","code"],{
    "command palette":C("CTRL_SHIFT","p"),"quick open":C("CTRL","p"),
    "settings":C("CTRL",","),"keyboard shortcuts":C("CTRL_K","CTRL_S"),
    "toggle sidebar":C("CTRL","b"),"toggle terminal":C("CTRL","`"),"toggle panel":C("CTRL","j"),
    "explorer":C("CTRL_SHIFT","e"),"search files":C("CTRL_SHIFT","f"),
    "source control":C("CTRL_SHIFT","g"),"extensions":C("CTRL_SHIFT","x"),
    "new file":C("CTRL","n"),"save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),
    "close tab":C("CTRL","w"),"reopen tab":C("CTRL_SHIFT","t"),
    "split editor":C("CTRL","\\"),"close all":C("CTRL_K","CTRL_W"),
    "zen mode":C("CTRL_K","Z"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "cut line":C("CTRL","x"),"copy line":C("CTRL","c"),"paste":C("CTRL","v"),
    "move line up":C("ALT","UP"),"move line down":C("ALT","DOWN"),
    "copy line up":C("ALT_SHIFT","UP"),"copy line down":C("ALT_SHIFT","DOWN"),
    "delete line":C("CTRL_SHIFT","k"),
    "insert line below":C("CTRL","ENTER"),"insert line above":C("CTRL_SHIFT","ENTER"),
    "toggle comment":C("CTRL","/"),"toggle block comment":C("SHIFT_ALT","a"),
    "indent":C("CTRL","]"),"outdent":C("CTRL","["),
    "add cursor below":C("CTRL_ALT","DOWN"),"add cursor above":C("CTRL_ALT","UP"),
    "add selection next":C("CTRL","d"),"select all occurences":C("CTRL_SHIFT","l"),
    "go to line":C("CTRL","g"),"go to symbol":C("CTRL_SHIFT","o"),
    "go to definition":K("F12"),"peek definition":C("ALT","F12"),
    "go to references":C("SHIFT","F12"),
    "rename symbol":K("F2"),
    "format document":C("ALT_SHIFT","f"),
    "find":C("CTRL","f"),"find replace":C("CTRL","h"),
    "find in files":C("CTRL_SHIFT","f"),"replace in files":C("CTRL_SHIFT","h"),
    "start debug":K("F5"),"stop debug":C("SHIFT","F5"),"step over":K("F10"),
    "step into":K("F11"),"step out":C("SHIFT","F11"),
    "toggle breakpoint":K("F9"),
    "markdown preview":C("CTRL_SHIFT","v"),
    "word wrap":C("ALT","z")
},{
    "command palette":C("GUI_SHIFT","p"),"quick open":C("GUI","p"),
    "settings":C("GUI",","),"keyboard shortcuts":C("GUI_K","GUI_S"),
    "toggle sidebar":C("GUI","b"),"toggle terminal":C("GUI","`"),"toggle panel":C("GUI","j"),
    "explorer":C("GUI_SHIFT","e"),"search files":C("GUI_SHIFT","f"),
    "source control":C("GUI_SHIFT","g"),"extensions":C("GUI_SHIFT","x"),
    "new file":C("GUI","n"),"save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "close tab":C("GUI","w"),"reopen tab":C("GUI_SHIFT","t"),
    "split editor":C("GUI","\\"),"zen mode":C("GUI_K","Z"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "cut line":C("GUI","x"),"copy line":C("GUI","c"),"paste":C("GUI","v"),
    "move line up":C("ALT","UP"),"move line down":C("ALT","DOWN"),
    "copy line up":C("ALT_SHIFT","UP"),"copy line down":C("ALT_SHIFT","DOWN"),
    "delete line":C("GUI_SHIFT","k"),
    "insert line below":C("GUI","ENTER"),"insert line above":C("GUI_SHIFT","ENTER"),
    "toggle comment":C("GUI","/"),"toggle block comment":C("SHIFT_ALT","a"),
    "indent":C("GUI","]"),"outdent":C("GUI","["),
    "add cursor below":C("GUI_ALT","DOWN"),"add cursor above":C("GUI_ALT","UP"),
    "add selection next":C("GUI","d"),"select all occurences":C("GUI_SHIFT","l"),
    "go to line":C("GUI","g"),"go to symbol":C("GUI_SHIFT","o"),
    "go to definition":K("F12"),"peek definition":C("ALT","F12"),
    "go to references":C("SHIFT","F12"),
    "rename symbol":K("F2"),
    "format document":C("ALT_SHIFT","f"),
    "find":C("GUI","f"),"find replace":C("GUI","h"),
    "find in files":C("GUI_SHIFT","f"),"replace in files":C("GUI_SHIFT","h"),
    "start debug":K("F5"),"stop debug":C("SHIFT","F5"),"step over":K("F10"),
    "step into":K("F11"),"step out":C("SHIFT","F11"),
    "toggle breakpoint":K("F9"),
    "markdown preview":C("GUI_SHIFT","v"),
    "word wrap":C("GUI","z")
})

# ── OBSIDIAN ──
app("obsidian",["obsdn","obs"],{
    "quick switcher":C("CTRL","o"),"search":C("CTRL_SHIFT","f"),
    "back":C("CTRL","ALT_LEFT"),"forward":C("CTRL","ALT_RIGHT"),
    "open note":C("CTRL","o"),"new note":C("CTRL","n"),
    "graph view":C("CTRL","g"),"backlinks":C("CTRL","b"),
    "toggle sidebar":C("CTRL","\\\\"),"toggle preview":C("CTRL","e"),
    "bold":C("CTRL","b"),"italic":C("CTRL","i"),"highlights":C("CTRL_SHIFT","h"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "indent":C("CTRL","TAB"),"outdent":C("CTRL_SHIFT","TAB"),
    "find":C("CTRL","f"),"find replace":C("CTRL","h"),
    "save":C("CTRL","s"),"new tab":C("CTRL","t"),
    "command palette":C("CTRL","p")
},{
    "quick switcher":C("GUI","o"),"search":C("GUI_SHIFT","f"),
    "back":C("GUI","["),"forward":C("GUI","]"),
    "open note":C("GUI","o"),"new note":C("GUI","n"),
    "graph view":C("GUI","g"),"backlinks":C("GUI","b"),
    "toggle sidebar":C("GUI","\\\\"),"toggle preview":C("GUI","e"),
    "bold":C("GUI","b"),"italic":C("GUI","i"),"highlights":C("GUI_SHIFT","h"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "indent":C("GUI","TAB"),"outdent":C("GUI_SHIFT","TAB"),
    "find":C("GUI","f"),"find replace":C("GUI","h"),
    "save":C("GUI","s"),"new tab":C("GUI","t"),
    "command palette":C("GUI","p")
})

# ── NOTION ──
app("notion",["ntn"],{
    "quick find":C("CTRL","p"),"search":C("CTRL","f"),
    "new page":C("CTRL","n"),"new database":C("CTRL_SHIFT","n"),
    "bold":C("CTRL","b"),"italic":C("CTRL","i"),"underline":C("CTRL","u"),"strikethrough":C("CTRL_SHIFT","s"),
    "code":C("CTRL","e"),"inline code":C("CTRL","e"),"toggle todo":C("CTRL","ENTER"),
    "back":C("CTRL","["),"forward":C("CTRL","]"),
    "undo":C("CTRL","z"),"redo":C("CTRL_SHIFT","z"),
    "create comment":C("CTRL_SHIFT","m"),"toggle sidebar":C("CTRL","\\\\"),
    "small text":C("CTRL_SHIFT","."),"large text":C("CTRL_SHIFT",","),
    "help":C("CTRL","/")
},{
    "quick find":C("GUI","p"),"search":C("GUI","f"),
    "new page":C("GUI","n"),"new database":C("GUI_SHIFT","n"),
    "bold":C("GUI","b"),"italic":C("GUI","i"),"underline":C("GUI","u"),"strikethrough":C("GUI_SHIFT","s"),
    "code":C("GUI","e"),"inline code":C("GUI","e"),"toggle todo":C("GUI","ENTER"),
    "back":C("GUI","["),"forward":C("GUI","]"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "create comment":C("GUI_SHIFT","m"),"toggle sidebar":C("GUI","\\\\"),
    "small text":C("GUI_SHIFT","."),"large text":C("GUI_SHIFT",","),
    "help":C("GUI","/")
})

# ── MICROSOFT EXCEL ──
app("excel",["microsoft excel","ms excel"],{
    "new workbook":C("CTRL","n"),"open":C("CTRL","o"),"save":C("CTRL","s"),"save as":K("F12"),
    "undo":C("CTRL","z"),"redo":C("CTRL","y"),
    "copy":C("CTRL","c"),"cut":C("CTRL","x"),"paste":C("CTRL","v"),
    "find":C("CTRL","f"),"find replace":C("CTRL","h"),
    "select all":C("CTRL","a"),"select column":C("CTRL","SPACE"),"select row":C("SHIFT","SPACE"),
    "bold":C("CTRL","b"),"italic":C("CTRL","i"),"underline":C("CTRL","u"),
    "go to":C("CTRL","g"),"go to cell":C("CTRL","g"),
    "insert chart":K("F11"),"insert function":C("SHIFT","F3"),
    "create table":C("CTRL","t"),"create pivot":C("ALT","n"),"sum":C("ALT","="),
    "format cells":C("CTRL","1"),"fill right":C("CTRL","r"),"fill down":C("CTRL","d"),
    "filter":C("CTRL_SHIFT","l"),"sort ascending":C("ALT_SHIFT","s"),
    "group":C("ALT_SHIFT","RIGHT"),"ungroup":C("ALT_SHIFT","LEFT"),
    "add comment":C("SHIFT","F2"),
    "protect":C("ALT_R","P"),"print":C("CTRL","p")
},{
    "new workbook":C("GUI","n"),"open":C("GUI","o"),"save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "undo":C("GUI","z"),"redo":C("GUI","y"),
    "copy":C("GUI","c"),"cut":C("GUI","x"),"paste":C("GUI","v"),
    "find":C("GUI","f"),"find replace":C("GUI","h"),
    "select all":C("GUI","a"),"bold":C("GUI","b"),"italic":C("GUI","i"),"underline":C("GUI","u"),
    "go to":C("GUI","g"),"insert function":C("SHIFT","F3"),
    "create table":C("GUI","t"),"format cells":C("GUI","1"),
    "fill right":C("GUI","r"),"fill down":C("GUI","d"),
    "filter":C("GUI_SHIFT","l"),"print":C("GUI","p")
})

# ── MICROSOFT OUTLOOK ──
app("outlook",["microsoft outlook","ms outlook"],{
    "new email":C("CTRL","n"),"send":C("CTRL","ENTER"),
    "reply":C("CTRL","r"),"reply all":C("CTRL_SHIFT","r"),"forward":C("CTRL","f"),
    "delete":C("CTRL","d"),"archive":C("CTRL","e"),
    "flag":C("CTRL_SHIFT","g"),"mark read":C("CTRL","q"),"mark unread":C("CTRL","u"),
    "go to inbox":C("CTRL_SHIFT","i"),"go to calendar":C("CTRL","2"),
    "go to contacts":C("CTRL","3"),"go to tasks":C("CTRL","4"),
    "search":C("CTRL","e"),"address book":C("CTRL_SHIFT","b"),
    "save":C("CTRL","s"),"print":C("CTRL","p"),
    "zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),
    "check names":C("CTRL","k"),"attach file":C("CTRL_SHIFT","a"),
    "new appointment":C("CTRL_SHIFT","a"),"new meeting":C("CTRL_SHIFT","q"),
    "new contact":C("CTRL_SHIFT","c"),"new task":C("CTRL_SHIFT","k")
},{
    "new email":C("GUI","n"),"send":C("GUI","ENTER"),
    "reply":C("GUI","r"),"reply all":C("GUI_SHIFT","r"),"forward":C("GUI","f"),
    "delete":C("GUI","d"),"archive":C("GUI","e"),
    "flag":C("GUI_SHIFT","g"),"mark read":C("GUI","q"),"mark unread":C("GUI","u"),
    "go to inbox":C("GUI_SHIFT","i"),"search":C("GUI","e"),
    "save":C("GUI","s"),"print":C("GUI","p"),
    "zoom in":C("GUI","="),"zoom out":C("GUI","-"),
    "attach file":C("GUI_SHIFT","a")
})

# ── NOTEPAD ──
app("notepad",["np","notepad++","npp"],{
    "new":C("CTRL","n"),"new file":C("CTRL","n"),
    "open":C("CTRL","o"),"open file":C("CTRL","o"),
    "save":C("CTRL","s"),"save as":C("CTRL_SHIFT","s"),
    "print":C("CTRL","p"),
    "undo":C("CTRL","z"),"redo":C("CTRL","y"),
    "cut":C("CTRL","x"),"copy":C("CTRL","c"),"paste":C("CTRL","v"),
    "delete":K("DELETE"),"select all":C("CTRL","a"),
    "find":C("CTRL","f"),"find next":K("F3"),"find prev":C("SHIFT","F3"),
    "replace":C("CTRL","h"),
    "go to":C("CTRL","g"),"go to line":C("CTRL","g"),
    "time date":K("F5"),"insert time":K("F5"),
    "word wrap":C("CTRL","w"),"toggle wrap":C("CTRL","w"),
    "font":C("CTRL","="),"zoom in":C("CTRL","="),"zoom out":C("CTRL","-"),
    "status bar":C("CTRL","s"),"toggle status":C("CTRL","s")
},{
    "new":C("GUI","n"),"new file":C("GUI","n"),
    "open":C("GUI","o"),"open file":C("GUI","o"),
    "save":C("GUI","s"),"save as":C("GUI_SHIFT","s"),
    "print":C("GUI","p"),
    "undo":C("GUI","z"),"redo":C("GUI_SHIFT","z"),
    "cut":C("GUI","x"),"copy":C("GUI","c"),"paste":C("GUI","v"),
    "delete":K("DELETE"),"select all":C("GUI","a"),
    "find":C("GUI","f"),"find next":K("F3"),"find prev":C("SHIFT","F3"),
    "replace":C("GUI","h"),
    "go to":C("GUI","g"),"go to line":C("GUI","g"),
    "time date":K("F5"),"insert time":K("F5"),
    "word wrap":C("GUI","w"),"toggle wrap":C("GUI","w"),
    "font":C("GUI","="),"zoom in":C("GUI","="),"zoom out":C("GUI","-")
})

# ── WINDOWS 11 ──
app("windows",["windows 11","win","windows os"],{
    "lock":C("GUI","l"),"screenshot snip":C("GUI_SHIFT","s"),
    "full screenshot":C("GUI","PRINTSCREEN"),"snipping tool":C("GUI_SHIFT","s"),
    "clipboard":C("GUI","v"),"task manager":C("CTRL_SHIFT","ESC"),
    "task view":C("GUI","TAB"),"new desktop":C("GUI_CTRL","d"),
    "switch desktop right":C("GUI_CTRL","RIGHT"),"switch desktop left":C("GUI_CTRL","LEFT"),
    "snap left":C("GUI","LEFT"),"snap right":C("GUI","RIGHT"),
    "snap up":C("GUI","UP"),"snap down":C("GUI","DOWN"),
    "file explorer":C("GUI","e"),"run":C("GUI","r"),"settings":C("GUI","i"),
    "search":C("GUI","s"),"show desktop":C("GUI","d"),
    "minimize all":C("GUI","d"),"project":C("GUI","p"),
    "emoji panel":C("GUI","."),"magnifier":C("GUI","="),
    "narrator":C("GUI","ENTER"),"game bar":C("GUI","g"),
    "action center":C("GUI","a"),"quick settings":C("GUI","a"),
    "lock orientation":C("GUI","o"),
    "record screen":C("GUI","ALT_r"),"screenshot":C("GUI","PRINTSCREEN"),
    "minimize window":C("GUI","DOWN"),"maximize window":C("GUI","UP")
})

# ── MACOS ──
app("macos",["mac","mac os","apple"],{
    "lock screen":C("CTRL","GUI","q"),"screenshot full":C("GUI_SHIFT","3"),
    "screenshot area":C("GUI_SHIFT","4"),"screenshot window":C("GUI_SHIFT","4","SPACE"),
    "screen recording":C("GUI_SHIFT","5"),
    "spotlight":C("GUI","SPACE"),"finder":C("GUI","TAB"),
    "force quit":C("GUI_ALT","ESC"),
    "minimize":C("GUI","m"),"hide app":C("GUI","h"),"hide others":C("GUI_ALT","h"),
    "quit app":C("GUI","q"),"close window":C("GUI","w"),
    "switch app":C("GUI","TAB"),"switch window same app":C("GUI","`"),
    "mission control":C("CTRL","UP"),"app expose":C("CTRL","DOWN"),
    "new desktop":C("CTRL","RIGHT"),"prev desktop":C("CTRL","LEFT"),
    "show desktop":K("F11"),"launchpad":K("F4"),
    "emojis":C("CTRL","GUI","SPACE"),
    "fullscreen":C("CTRL","GUI","f"),
    "preferences":C("GUI",","),
    "trash":C("GUI","DELETE"),"force empty trash":C("GUI_SHIFT","DELETE"),
    "eject":C("GUI","e"),"rename":K("ENTER"),
    "get info":C("GUI","i"),"finder new window":C("GUI","n"),
    "finder search":C("GUI","f"),"finder go":C("GUI_SHIFT","g")
})

# ── Generate JS ──
def js_val(v, indent=0):
    prefix = "  " * indent
    inner = "  " * (indent + 1)
    if isinstance(v, dict):
        if not v:
            return "{}"
        items = []
        for k, val in v.items():
            items.append(f'{inner}"{k}": {js_val(val, indent + 1)}')
        return "{\n" + ",\n".join(items) + f"\n{prefix}}}"
    elif isinstance(v, list):
        if not v:
            return "[]"
        items = [f'{inner}"{item}"' for item in v]
        return "[\n" + ",\n".join(items) + f"\n{prefix}]"
    elif isinstance(v, str):
        return json.dumps(v)
    elif isinstance(v, bool):
        return "true" if v else "false"
    elif isinstance(v, int):
        return str(v)
    return "null"

output = ["var SHORTCUTS = {"]
entries = []
for name, data in SC.items():
    entry = f'  "{name}": {{\n    "aliases": {js_val(data["aliases"], 1)},\n    "windows": {js_val(data["windows"], 1)},\n    "macos": {js_val(data["macos"], 1)}\n  }}'
    entries.append(entry)
output.append(",\n".join(entries))
output.append("};\n")
output.append("\n(function(){\n")
output.append("  for(var k in SHORTCUTS){\n")
output.append("    if(!SHORTCUTS[k].linux) SHORTCUTS[k].linux = SHORTCUTS[k].windows;\n")
output.append("  }\n")
output.append("})();\n")

# Also add GENERIC_ACTIONS
output.append("\nvar GENERIC_ACTIONS = {\n")
gen = {
    "copy": C("CTRL","c"),"paste": C("CTRL","v"),"cut": C("CTRL","x"),"save": C("CTRL","s"),
    "find": C("CTRL","f"),"select all": C("CTRL","a"),"undo": C("CTRL","z"),"redo": C("CTRL","y"),
    "screenshot": C("GUI_SHIFT","s"),"lock": C("GUI","l"),
    "play": K("MEDIA_PLAY_PAUSE"),"pause": K("MEDIA_PLAY_PAUSE"),"play pause": K("MEDIA_PLAY_PAUSE"),
    "next": K("MEDIA_NEXT_TRACK"),"next track": K("MEDIA_NEXT_TRACK"),
    "previous": K("MEDIA_PREV_TRACK"),"prev": K("MEDIA_PREV_TRACK"),
    "mute": K("MEDIA_MUTE"),"volume up": K("MEDIA_VOLUME_UP"),"volume down": K("MEDIA_VOLUME_DOWN"),
    "enter": K("ENTER"),"tab": K("TAB"),"escape": K("ESC"),"esc": K("ESC"),
    "space": K("SPACE"),"delete": K("DELETE"),"backspace": K("BACKSPACE"),
    "home": K("HOME"),"end": K("END"),"page up": K("PAGE_UP"),"page down": K("PAGE_DOWN"),
}
for i, (k, v) in enumerate(gen.items()):
    comma = "," if i < len(gen) - 1 else ""
    output.append(f'  "{k}": {js_val(v, 1)}{comma}\n')
output.append("};\n")

# Mac versions of generic actions
output.append("\nvar GENERIC_ACTIONS_MAC = {\n")
gen_mac = {k: C(v["mod"].replace("CTRL","GUI"), v["key"]) if v.get("type") == "combo" else v for k,v in gen.items()}
gen_mac["screenshot"] = C("GUI_SHIFT","4")
gen_mac["lock"] = C("CTRL_GUI","q")
for i, (k, v) in enumerate(gen_mac.items()):
    comma = "," if i < len(gen_mac) - 1 else ""
    output.append(f'  "{k}": {js_val(v, 1)}{comma}\n')
output.append("};\n")

with open("/home/sudobreakstuff/sudodeck/docs/shortcuts.js", "w") as f:
    f.write("".join(output))

print(f"Wrote {len(SC)} apps, file size: {sum(len(l) for l in output)} bytes")
