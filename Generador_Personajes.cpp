#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Paths
static const char *CSV_PATH  = "C:\\Users\\Acer\\Desktop\\IMMOA\\csv_y_txt\\characters.csv";
static const char *DICT_PATH = "C:\\Users\\Acer\\Desktop\\IMMOA\\csv_y_txt\\dictionary.txt";

// --- Random helpers ---
static int rand_in(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

static int roll_4d6_drop_lowest(void) {
    int rolls[4], sum = 0, low = 7;
    for (int i = 0; i < 4; ++i) {
        rolls[i] = rand_in(1, 6);
        sum += rolls[i];
        if (rolls[i] < low) low = rolls[i];
    }
    return sum - low;
}

static int ability_mod(int score) { return (score - 10) / 2; }

static void now_timestamp(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm *tmv = localtime(&t);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", tmv);
}

// --- Data pools ---
static const char *FIRST_NAMES[] = {
    "Arthas","Lyra","Kael","Roran","Selene","Thane","Mira","Korin","Elara","Darius",
    "Luna","Galen","Seren","Rowan","Eryndor","Kara","Lucan","Nyra","Thalos","Rhea"
};
static const int FIRST_N = sizeof(FIRST_NAMES)/sizeof(FIRST_NAMES[0]);

static const char *LAST_NAMES[] = {
    "Stormwind","Dawnsong","Ravencrest","Ironfist","Moonscar","Duskbane",
    "Brightshield","Emberfall","Windrider","Nightbloom"
};
static const int LAST_N = sizeof(LAST_NAMES)/sizeof(LAST_NAMES[0]);

static const char *RACES[] = {"Human","Elf","Dwarf","Halfling","Gnome","HalfOrc","Tiefling","Dragonborn"};
static const int RACES_N = sizeof(RACES)/sizeof(RACES[0]);

// Classes (D&D 5e)
static const char *CLASS_NAMES[] = {
    "Barbarian","Fighter","Paladin","Ranger",
    "Artificer","Bard","Cleric","Druid","Monk","Rogue","Warlock","Sorcerer","Wizard"
};
static const int CLASS_HIT_DICE[] = {12,10,10,10,8,8,8,8,8,8,8,6,6};
static const int CLASS_COUNT = sizeof(CLASS_NAMES)/sizeof(CLASS_NAMES[0]);

// Proficiencies
static const char *ARMOR_OPTS[] = {"LightArmor","MediumArmor","HeavyArmor","Shields"};
static const int ARMOR_N = sizeof(ARMOR_OPTS)/sizeof(ARMOR_OPTS[0]);
static const char *WEAPON_OPTS[] = {"SimpleWeapons","MartialWeapons","Swords","Axes","Bows","Daggers","Spears","Maces"};
static const int WEAPON_N = sizeof(WEAPON_OPTS)/sizeof(WEAPON_OPTS[0]);
static const char *TOOL_OPTS[] = {"SmithTools","ThievesTools","AlchemistKit","HerbalismKit","NavigatorTools","TinkerTools","PoisonersKit"};
static const int TOOL_N = sizeof(TOOL_OPTS)/sizeof(TOOL_OPTS[0]);
static const char *SAVE_OPTS[] = {"STR","DEX","CON","INT","WIS","CHA"};
static const int SAVE_N = sizeof(SAVE_OPTS)/sizeof(SAVE_OPTS[0]);
static const char *SKILL_OPTS[] = {
    "Acrobatics","AnimalHandling","Arcana","Athletics","Deception","History",
    "Insight","Intimidation","Investigation","Medicine","Nature","Perception",
    "Performance","Persuasion","Religion","SleightOfHand","Stealth","Survival"
};
static const int SKILL_N = sizeof(SKILL_OPTS)/sizeof(SKILL_OPTS[0]);

// Random fantasy item/spell descriptions
static const char *ITEM_DESCS[] = {
    "Forged by dwarves deep beneath the mountain.",
    "An ancient relic humming with faint energy.",
    "Covered in strange runes that glow when held.",
    "A weapon that whispers the name of its enemies.",
    "A tool once owned by a legendary hero."
};
static const int ITEM_DESC_N = sizeof(ITEM_DESCS)/sizeof(ITEM_DESCS[0]);

static const char *SPELL_DESCS[] = {
    "A burst of flame erupts from your hand.",
    "A shimmering barrier surrounds the target.",
    "A spectral hand floats and manipulates objects.",
    "Bolts of arcane energy strike from the sky.",
    "A chilling frost covers the ground in front of you."
};
static const int SPELL_DESC_N = sizeof(SPELL_DESCS)/sizeof(SPELL_DESCS[0]);

// Helpers
static void pick_two_distinct(int n, int *a, int *b) {
    *a = rand_in(0, n-1);
    do { *b = rand_in(0, n-1); } while (*b == *a);
}

static void random_name(char *buf, size_t n) {
    snprintf(buf, n, "%s %s",
             FIRST_NAMES[rand_in(0, FIRST_N-1)],
             LAST_NAMES[rand_in(0, LAST_N-1)]);
}

// --- Character writer ---
static void write_one_character(FILE *csv, int seq_id) {
    char name[64]; random_name(name, sizeof(name));
    const char *race = RACES[rand_in(0, RACES_N-1)];
    int gender = rand_in(0, 1);
    int height = rand_in(140, 200);
    int speed  = rand_in(25, 35);

    // Attributes
    int STR = roll_4d6_drop_lowest();
    int DEX = roll_4d6_drop_lowest();
    int CON = roll_4d6_drop_lowest();
    int INTL = roll_4d6_drop_lowest();
    int WIS = roll_4d6_drop_lowest();
    int CHA = roll_4d6_drop_lowest();

    // Class + HP
    int class_idx = rand_in(0, CLASS_COUNT - 1);
    const char *class_name = CLASS_NAMES[class_idx];
    int class_hitdie = CLASS_HIT_DICE[class_idx];
    int hp_hitdie = class_hitdie;
    int hp_conmod = ability_mod(CON);

    // AOE
    int aoe_shape = rand_in(0, 3);
    int aoe_size  = rand_in(5, 30);
    int aoe_move  = rand_in(0, 1);

    // Component
    int comp_cons = rand_in(0, 1);
    int comp_cost = rand_in(0, 200);

    // Ability / Effect
    int ability_perm = rand_in(0, 1);
    int effect_perm  = rand_in(0, 1);

    // Object
    int  obj_range = rand_in(5, 60);
    int  obj_def   = rand_in(0, 5);
    const char *obj_cat_opts[] = {"Weapon","Armor","Tool","MagicItem"};
    const char *obj_cat = obj_cat_opts[rand_in(0, 3)];
    int  obj_conc = rand_in(0, 1);
    const char *obj_desc = ITEM_DESCS[rand_in(0, ITEM_DESC_N-1)];

    // Spell
    int  spell_range = rand_in(10, 120);
    int  spell_conc  = rand_in(0, 1);
    const char *spell_desc = SPELL_DESCS[rand_in(0, SPELL_DESC_N-1)];

    // Proficiencies (2 per type)
    int a1,a2,w1,w2,t1,t2,sav1,sav2,sk1,sk2;
    pick_two_distinct(ARMOR_N, &a1, &a2);
    pick_two_distinct(WEAPON_N, &w1, &w2);
    pick_two_distinct(TOOL_N, &t1, &t2);
    pick_two_distinct(SAVE_N, &sav1, &sav2);
    pick_two_distinct(SKILL_N, &sk1, &sk2);

    // Base info (at end)
    char base_name[32]; snprintf(base_name, sizeof(base_name), "Base_%d", seq_id);
    int base_id = seq_id;
    char base_ts[32]; now_timestamp(base_ts, sizeof(base_ts));

    fprintf(csv,
        "%s,%s,%d,%d,%d,"
        "%d,%d,%d,%d,%d,%d,"
        "%s,%d,%d,%d,"
        "%d,%d,%d,"
        "%d,%d,"
        "%d,%d,"
        "%d,%d,%s,%d,%s,"
        "%d,%d,%s,"
        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
        "%s,%d,%s\n",
        name, race, gender, height, speed,
        STR, DEX, CON, INTL, WIS, CHA,
        class_name, class_hitdie, hp_hitdie, hp_conmod,
        aoe_shape, aoe_size, aoe_move,
        comp_cons, comp_cost,
        ability_perm, effect_perm,
        obj_range, obj_def, obj_cat, obj_conc, obj_desc,
        spell_range, spell_conc, spell_desc,
        ARMOR_OPTS[a1], ARMOR_OPTS[a2],
        WEAPON_OPTS[w1], WEAPON_OPTS[w2],
        TOOL_OPTS[t1], TOOL_OPTS[t2],
        SAVE_OPTS[sav1], SAVE_OPTS[sav2],
        SKILL_OPTS[sk1], SKILL_OPTS[sk2],
        base_name, base_id, base_ts
    );
}

// --- Dictionary writer (now with index numbers) ---
static void write_dictionary(FILE *dict) {
    const char *text =
        "[00] character_name: generated fantasy name\n"
        "[01] race: character race\n"
        "[02] gender: 1=male, 0=female\n"
        "[03] height: height in cm\n"
        "[04] speed: feet per round\n"
        "[05] strength: ability score (4d6 drop lowest)\n"
        "[06] dexterity: ability score (4d6 drop lowest)\n"
        "[07] constitution: ability score (4d6 drop lowest)\n"
        "[08] intelligence: ability score (4d6 drop lowest)\n"
        "[09] wisdom: ability score (4d6 drop lowest)\n"
        "[10] charisma: ability score (4d6 drop lowest)\n"
        "[11] class_name: D&D 5e class\n"
        "[12] class_hitdie: hit die per class (Barbarian=12...Wizard=6)\n"
        "[13] hp_hitdie: same as class hit die\n"
        "[14] hp_conmod: constitution modifier\n"
        "[15] aoe_shape: area shape code (0–3)\n"
        "[16] aoe_size: area size in feet\n"
        "[17] aoe_moveable: 1 if moves, 0 if static\n"
        "[18] component_consumable: 1 if consumed, 0 if not\n"
        "[19] component_cost: component cost (gp)\n"
        "[20] ability_permanent: 1 if permanent ability\n"
        "[21] effect_permanent: 1 if permanent effect\n"
        "[22] object_range: object effective range\n"
        "[23] object_defense: defensive value (0–5)\n"
        "[24] object_category: Weapon, Armor, Tool, or MagicItem\n"
        "[25] object_concentration: 1 if requires concentration\n"
        "[26] object_description: flavor text\n"
        "[27] spell_range: spell range in feet\n"
        "[28] spell_concentration: 1 if requires concentration\n"
        "[29] spell_description: flavor text\n"
        "[30–31] armor_prof1–2: armor proficiencies\n"
        "[32–33] weapon_prof1–2: weapon proficiencies\n"
        "[34–35] tool_prof1–2: tool proficiencies\n"
        "[36–37] savingthrow_prof1–2: saving throw proficiencies\n"
        "[38–39] skill_prof1–2: skill proficiencies\n"
        "[40] base_name: internal base ID name\n"
        "[41] base_id: numeric ID\n"
        "[42] base_timestamp: generation timestamp\n";
    fputs(text, dict);
}

int main(void) {
    srand((unsigned)time(NULL));
    int NUM_CHARACTERS;
    printf("Insert number of characters to generate: ");
    scanf("%d", &NUM_CHARACTERS);

    FILE *csv = fopen(CSV_PATH, "w");
    FILE *dict = fopen(DICT_PATH, "w");

    if (!csv || !dict) {
        fprintf(stderr, "Error: could not open output files.\n");
        if (csv) fclose(csv);
        if (dict) fclose(dict);
        return 1;
    }

    write_dictionary(dict);
    fclose(dict);

    for (int i = 1; i <= NUM_CHARACTERS; ++i)
        write_one_character(csv, i);

    fclose(csv);
    printf("Done! Files saved in: C:\\Users\\Acer\\Desktop\\IMMOA\\csv_y_txt\\\n");
    return 0;
}

