from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[2]
BASE = ROOT / "Fontes" / "MapDesign" / "SpellRise_Elarion_Satellite_Concept_v01.png"
OUT = ROOT / "Fontes" / "MapDesign" / "SpellRise_Elarion_Settlements_v01.png"


def font(size, bold=False):
    candidates = [
        Path("C:/Windows/Fonts/arialbd.ttf" if bold else "C:/Windows/Fonts/arial.ttf"),
        Path("C:/Windows/Fonts/segoeuib.ttf" if bold else "C:/Windows/Fonts/segoeui.ttf"),
    ]
    for candidate in candidates:
        if candidate.exists():
            return ImageFont.truetype(str(candidate), size)
    return ImageFont.load_default()


RACIAL_CAPITALS = [
    ("Aurelthar", 456, 305, "Refugio Noroeste"),
    ("Khar Veyr", 987, 318, "Refugio das Alturas"),
    ("Mournhal", 438, 791, "Refugio do Brejo"),
    ("Vhar Drakken", 987, 777, "Refugio das Cinzas"),
]

RACIAL_TOWNSHIPS = [
    ("Eldermoss", 379, 255), ("Dunhollow", 529, 366), ("Caer Nival", 610, 250),
    ("Stonevein", 923, 249), ("Highwrit", 1064, 397), ("Karnwatch", 1108, 300),
    ("Reedmere", 340, 725), ("Blackfen", 545, 841), ("Saltmere", 330, 842),
    ("Ashbridge", 894, 699), ("Cinderwold", 1084, 843), ("Ironwake", 1142, 758),
]

CLAN_CITIES = [
    ("Grimhold", 609, 417), ("Vaelgate", 704, 350), ("Ravencrest", 780, 438),
    ("Thornwall", 633, 545), ("Oathscar", 824, 539), ("Greybarrow", 529, 636),
    ("Duskmarch", 718, 646), ("Harrowfen", 458, 656), ("Redspire", 880, 618),
    ("Blackreach", 950, 565), ("Glasswatch", 1130, 214), ("Saltcrown", 1249, 309),
    ("Dunegrave", 1176, 370), ("Frostmere", 499, 108), ("Aurorath", 580, 170),
    ("Wolfskein", 414, 166), ("Rootmaw", 252, 732), ("Viridian Hollow", 312, 843),
    ("Mirecrown", 203, 815), ("Cinderfall", 1208, 782), ("Emberkeep", 1303, 857),
    ("Obsidian Gate", 1174, 909), ("Stormharbor", 695, 805), ("Widowcliff", 1020, 650),
]

HAMLETS = [
    ("Briargate", 407, 336), ("Old Hush", 502, 276), ("Mothwell", 563, 319),
    ("Pale Orchard", 674, 286), ("Crowford", 738, 383), ("Hollow Cross", 582, 470),
    ("Sable Mill", 489, 498), ("Wayfen", 418, 566), ("Gallowmere", 541, 585),
    ("Rookstead", 656, 610), ("Low Cairn", 750, 589), ("Witchford", 838, 464),
    ("Karn Hollow", 944, 424), ("Red Quarry", 1018, 470), ("Thane's Rest", 1072, 545),
    ("Drymark", 1117, 250), ("Sunken Spire", 1225, 250), ("Glassmere", 1310, 355),
    ("Dunescar", 1165, 460), ("Frosthook", 466, 197), ("White Cairn", 551, 139),
    ("Icewake", 634, 200), ("Mossgrave", 275, 666), ("Rootford", 348, 698),
    ("Green Wound", 404, 862), ("Fenwatch", 500, 736), ("Mudspire", 561, 799),
    ("Saltroot", 287, 898), ("Ashmere", 855, 744), ("Cinder Pike", 941, 831),
    ("Varr's Kiln", 1035, 873), ("Blackglass", 1120, 718), ("Molten Choir", 1264, 924),
    ("Char Harbor", 1338, 790), ("Bone Shoal", 781, 800), ("Wyrm Ford", 927, 615),
]

VILLAGES = [
    ("Mirebell", 373, 393), ("Foxgrave", 448, 421), ("Sorrowbrook", 533, 432),
    ("Dewrot", 615, 380), ("Hearthmere", 681, 438), ("Crowsalt", 756, 484),
    ("Wick Hollow", 593, 675), ("Bramble Low", 667, 718), ("Tarnfield", 498, 707),
    ("Ossuary Bend", 804, 710), ("Eastmere", 918, 515), ("Granite Fold", 1002, 380),
    ("Veinford", 1056, 342), ("Khar Rest", 916, 296), ("Dustwater", 1200, 520),
    ("Veyr Salt", 1274, 415), ("Pale Dune", 1328, 286), ("Cold Lantern", 404, 107),
    ("Boreal Scar", 530, 218), ("Northwake", 635, 123), ("Rootbell", 178, 725),
    ("Mireglass", 234, 873), ("Old Grove", 341, 925), ("Vinebarrow", 456, 902),
    ("Reedwatch", 537, 695), ("Lowmire", 400, 779), ("Ashwold", 876, 897),
    ("Coalbrook", 958, 923), ("Emberwash", 1105, 805), ("Veyr Ash", 1177, 836),
    ("Drakken Rest", 1276, 742), ("Sootmere", 1357, 885), ("Deep Cleft", 850, 576),
    ("Liminal Ford", 765, 525), ("Anchorfall", 718, 477), ("Echo Basin", 868, 487),
    ("Black Star", 781, 561), ("Woundwatch", 904, 546), ("Deadwater", 719, 609),
    ("Grief Coast", 641, 792),
]


def draw_label(draw, xy, text, fill, size=18, bold=False, anchor="mm"):
    x, y = xy
    f = font(size, bold)
    bbox = draw.textbbox((x, y), text, font=f, anchor=anchor)
    pad = 4
    draw.rounded_rectangle(
        (bbox[0] - pad, bbox[1] - pad, bbox[2] + pad, bbox[3] + pad),
        radius=4,
        fill=(10, 12, 11, 175),
    )
    draw.text((x, y), text, font=f, fill=fill, anchor=anchor)


def dot(draw, x, y, radius, fill, outline=(10, 8, 6, 230), width=2):
    draw.ellipse((x - radius, y - radius, x + radius, y + radius), fill=fill, outline=outline, width=width)


def main():
    image = Image.open(BASE).convert("RGBA")
    overlay = Image.new("RGBA", image.size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(overlay)

    for _, x, y in VILLAGES:
        dot(draw, x, y, 4, (214, 190, 122, 230), width=1)

    for _, x, y in HAMLETS:
        dot(draw, x, y, 6, (191, 123, 68, 240), width=2)

    for name, x, y in CLAN_CITIES:
        dot(draw, x, y, 9, (173, 54, 46, 245), width=2)
        draw_label(draw, (x, y - 20), name, (244, 221, 179, 255), size=15, bold=True)

    for name, x, y in RACIAL_TOWNSHIPS:
        dot(draw, x, y, 8, (87, 155, 209, 245), width=2)

    for name, x, y, _ in RACIAL_CAPITALS:
        dot(draw, x, y, 14, (81, 211, 132, 255), width=3)
        draw_label(draw, (x, y - 30), name, (236, 255, 216, 255), size=20, bold=True)

    # Legenda compacta.
    lx, ly = 28, 28
    draw.rounded_rectangle((lx, ly, lx + 365, ly + 144), radius=8, fill=(7, 10, 10, 185), outline=(215, 190, 122, 190), width=2)
    draw.text((lx + 18, ly + 18), "Elarion - assentamentos v01", font=font(20, True), fill=(240, 224, 190, 255))
    legend = [
        ((81, 211, 132, 255), "Capital racial"),
        ((87, 155, 209, 245), "Vila racial / township"),
        ((173, 54, 46, 245), "Cidade de clã"),
        ((191, 123, 68, 240), "Hamlet de clã"),
        ((214, 190, 122, 230), "Vila comum / recurso"),
    ]
    for idx, (color, label) in enumerate(legend):
        yy = ly + 55 + idx * 18
        dot(draw, lx + 28, yy + 7, 5, color, width=1)
        draw.text((lx + 44, yy), label, font=font(14), fill=(220, 207, 177, 255))

    result = Image.alpha_composite(image, overlay)
    result.convert("RGB").save(OUT, quality=95)
    print(OUT)


if __name__ == "__main__":
    main()
