#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif

#define SCORE_FILE "brainvault_scores.txt"
#define NAME_LEN 40
#define MAX_LIVES 3

typedef struct {
    char text[180];
    char options[4][70];
    int answer;
} Question;

typedef struct {
    char name[NAME_LEN];
    int score;
    int lives;
    int level;
    int correct;
    int attempted;
} Player;

typedef struct { char name[NAME_LEN]; int score; } ScoreEntry;

static const Question knowledgeBank[] = {
    {"Which planet is known as the Red Planet?", {"Earth", "Mars", "Jupiter", "Venus"}, 2},
    {"What is the chemical symbol for gold?", {"Go", "Gd", "Au", "Ag"}, 3},
    {"Who wrote Romeo and Juliet?", {"Shakespeare", "Dickens", "Austen", "Orwell"}, 1},
    {"Which is the largest ocean?", {"Indian", "Atlantic", "Arctic", "Pacific"}, 4},
    {"How many bits make one byte?", {"4", "8", "16", "32"}, 2},
    {"What is the capital of Japan?", {"Seoul", "Kyoto", "Tokyo", "Beijing"}, 3},
    {"Which organ pumps blood through the body?", {"Lung", "Heart", "Liver", "Kidney"}, 2},
    {"What is the square root of 144?", {"10", "11", "12", "14"}, 3}
};

static const char *logicPrompt[] = {
    "Lion, Tiger, Leopard, Carrot - find the odd one.",
    "2, 4, 6, 9 - find the odd number.",
    "Circle, Square, Triangle, Blue - find the odd one.",
    "January, March, July, Monday - find the odd one."
};
static const char *logicAnswer[] = {"carrot", "9", "blue", "monday"};

void clearScreen(void) { printf("\033[2J\033[H"); }
void line(void) { puts("+--------------------------------------------------------------+"); }
void pauseGame(void) { char x[8]; printf("\nPress Enter to continue..."); fgets(x, sizeof x, stdin); }
void readLine(char *text, size_t size) {
    if (!fgets(text, (int)size, stdin)) { text[0] = '\0'; return; }
    text[strcspn(text, "\r\n")] = '\0';
}
int readNumber(const char *prompt, int low, int high) {
    char input[40]; long value; char *end;
    for (;;) {
        printf("%s", prompt); readLine(input, sizeof input);
        value = strtol(input, &end, 10);
        if (*input && *end == '\0' && value >= low && value <= high) return (int)value;
        printf("  Enter a number from %d to %d.\n", low, high);
    }
}
int equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        char ca = *a++, cb = *b++;
        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';
        if (ca != cb) return 0;
    }
    return *a == *b;
}
const char *rankFor(int score) {
    if (score <= 50) return "Beginner";
    if (score <= 100) return "Explorer";
    if (score <= 150) return "Scholar";
    if (score <= 200) return "Genius";
    return "BrainVault Master";
}
void status(const Player *p) {
    int i;
    printf("\nScore: %d  |  Door: %d/5  |  Lives: ", p->score, p->level);
    for (i = 0; i < p->lives; ++i) printf("<3 ");
    printf("\n");
}
void loadingScreen(void) {
    int i;
    printf("Initializing vault systems [");
    for (i = 0; i < 24; ++i) { printf("#"); fflush(stdout); SLEEP(35); }
    puts("] READY"); SLEEP(250);
}
void title(void) {
    clearScreen();
    puts("\n  ____             _       __     __         _ _");
    puts(" |  _ \\ _ __ __ _(_)_ __   \\ \\   / /_ _ _   _| | |_ ");
    puts(" | |_) | '__/ _` | | '_ \\   \\ \\ / / _` | | | | | __|");
    puts(" |  _ <| | | (_| | | | | |   \\ V / (_| | |_| | | |_ ");
    puts(" |_| \\_\\_|  \\__,_|_|_| |_|    \\_/ \\__,_|\\__,_|_|\\__|");
    puts("                 THE INTELLECTUAL ADVENTURE\n");
}
void instructions(void) {
    title(); line(); puts(" HOW TO PLAY"); line();
    puts("Open five doors in order. A correct answer earns 10 points.");
    puts("A wrong answer costs one life. Clear a door perfectly for +15.");
    puts("Door 1: memory | Door 2: knowledge | Door 3: logic");
    puts("Door 4: rapid fire | Door 5: the combined master vault.");
    puts("Type answers carefully; text answers are not case-sensitive."); pauseGame();
}
void credits(void) { title(); puts("BrainVault\nA first-year C programming project\nDesigned for curious Vault Explorers."); pauseGame(); }
void applyAnswer(Player *p, int correct) {
    p->attempted++;
    if (correct) { p->correct++; p->score += 10; puts("  ACCESS SIGNAL ACCEPTED! +10"); }
    else { p->lives--; puts("  ACCESS DENIED. One life lost."); }
}
int askQuestion(const Question *q, Player *p) {
    int i, answer;
    printf("\n%s\n", q->text);
    for (i = 0; i < 4; ++i) printf("  %d) %s\n", i + 1, q->options[i]);
    answer = readNumber("Choose 1-4: ", 1, 4);
    applyAnswer(p, answer == q->answer);
    return answer == q->answer;
}
int memoryChallenge(Player *p, int finalMode) {
    const char *shown[] = {"APPLE", "RIVER", "TRAIN", "MOON", "PENCIL"};
    const char *choices[] = {"APPLE", "RIVER", "PENCIL", "CLOUD"};
    int answer, i;
    line(); puts(finalMode ? " FINAL VAULT: MEMORY ECHO" : " DOOR 1: MEMORY LOCK"); line();
    puts("Memorize these vault fragments...");
    for (i = 0; i < 5; ++i) printf("   %s\n", shown[i]);
    SLEEP(2500); clearScreen();
    puts("Which word was NOT displayed?");
    for (i = 0; i < 4; ++i) printf("  %d) %s\n", i + 1, choices[i]);
    answer = readNumber("Your answer: ", 1, 4); applyAnswer(p, answer == 4);
    return answer == 4;
}
int knowledgeChallenge(Player *p) { return askQuestion(&knowledgeBank[rand() % 8], p); }
int logicChallenge(Player *p) {
    int n = rand() % 4; char answer[70];
    line(); puts(" DOOR 3: LOGIC LOCK"); line(); puts(logicPrompt[n]);
    printf("Answer: "); readLine(answer, sizeof answer);
    applyAnswer(p, equalsIgnoreCase(answer, logicAnswer[n]));
    return equalsIgnoreCase(answer, logicAnswer[n]);
}
int rapidFire(Player *p, int rounds) {
    int i, before = p->correct;
    line(); puts(" SPEED LOCK: RAPID FIRE"); line();
    puts("Answer quickly. Every correct signal strengthens the vault key.");
    for (i = 0; i < rounds && p->lives > 0; ++i) {
        printf("\n--- Pulse %d of %d ---\n", i + 1, rounds);
        askQuestion(&knowledgeBank[rand() % 8], p);
    }
    return p->correct - before;
}
int finishDoor(Player *p, int correctBefore, int attemptedBefore) {
    if (p->lives <= 0) return 0;
    if (p->correct > correctBefore && p->correct - correctBefore == p->attempted - attemptedBefore) { p->score += 15; puts("\nPerfect door bonus: +15 points!"); }
    printf("\nDOOR %d UNLOCKED! Current score: %d\n", p->level, p->score);
    p->level++; pauseGame(); return 1;
}
void vaultAnimation(void) {
    int i; clearScreen();
    for (i = 0; i < 3; ++i) { puts("        [========== BRAINVAULT ==========]"); puts("        |                                |"); puts("        |          * UNLOCKING *         |"); puts("        [================================]"); SLEEP(350); clearScreen(); }
    puts("\n        =================================="); puts("        ||   THE BRAINVAULT IS OPEN!   ||"); puts("        ||       KNOWLEDGE SECURED      ||"); puts("        ==================================");
}
void saveHighScore(const Player *p) { FILE *f = fopen(SCORE_FILE, "a"); if (f) { fprintf(f, "%s|%d\n", p->name, p->score); fclose(f); } }
int compareScores(const void *a, const void *b) { return ((const ScoreEntry *)b)->score - ((const ScoreEntry *)a)->score; }
void showHighScores(void) {
    FILE *f; ScoreEntry entries[100]; char buf[120], *sep; int count = 0, i;
    title(); line(); puts(" TOP VAULT EXPLORERS"); line(); f = fopen(SCORE_FILE, "r");
    if (!f) { puts("No scores saved yet. Be the first explorer!"); pauseGame(); return; }
    while (count < 100 && fgets(buf, sizeof buf, f)) { sep = strrchr(buf, '|'); if (sep) { *sep++ = '\0'; entries[count].score = atoi(sep); strncpy(entries[count].name, buf, NAME_LEN - 1); entries[count].name[NAME_LEN - 1] = '\0'; count++; } }
    fclose(f); qsort(entries, count, sizeof entries[0], compareScores);
    for (i = 0; i < count && i < 10; ++i) printf(" %2d. %-35s %4d\n", i + 1, entries[i].name, entries[i].score);
    if (!count) puts("No valid scores saved yet.");
    pauseGame();
}
void gameOver(Player *p) { title(); printf("VAULT EXPEDITION COMPLETE, %s\nScore: %d | Accuracy: %d%% | Rank: %s\n", p->name, p->score, p->attempted ? (100 * p->correct / p->attempted) : 0, rankFor(p->score)); saveHighScore(p); puts("Your score has been sealed in the Hall of Records."); pauseGame(); }
void startGame(const char *name) {
    Player p = {"", 0, MAX_LIVES, 1, 0, 0}; int before, attemptedBefore;
    strncpy(p.name, name, NAME_LEN - 1); title(); printf("Explorer %s, the legendary BrainVault awaits.\n", p.name); loadingScreen();
    before = p.correct; attemptedBefore = p.attempted; memoryChallenge(&p, 0); if (!finishDoor(&p, before, attemptedBefore)) { gameOver(&p); return; }
    before = p.correct; attemptedBefore = p.attempted; line(); puts(" DOOR 2: KNOWLEDGE LOCK"); line(); knowledgeChallenge(&p); if (!finishDoor(&p, before, attemptedBefore)) { gameOver(&p); return; }
    before = p.correct; attemptedBefore = p.attempted; logicChallenge(&p); if (!finishDoor(&p, before, attemptedBefore)) { gameOver(&p); return; }
    before = p.correct; attemptedBefore = p.attempted; rapidFire(&p, 3); if (!finishDoor(&p, before, attemptedBefore)) { gameOver(&p); return; }
    before = p.correct; title(); puts("DOOR 5: FINAL MASTER VAULT"); memoryChallenge(&p, 1); if (p.lives) knowledgeChallenge(&p); if (p.lives) logicChallenge(&p); if (p.lives) rapidFire(&p, 2);
    if (p.lives > 0) { p.score += 25; puts("\nFINAL VAULT BONUS: +25 points!"); vaultAnimation(); }
    gameOver(&p);
}
int main(void) {
    char name[NAME_LEN]; int choice;
    srand((unsigned)time(NULL)); title(); loadingScreen();
    do { printf("Enter your Vault Explorer name: "); readLine(name, sizeof name); } while (!name[0]);
    for (;;) { title(); printf("Welcome, %s\n", name); line(); puts(" 1. Start Game\n 2. Instructions\n 3. High Scores\n 4. Credits\n 5. Exit"); line(); choice = readNumber("Select: ", 1, 5);
        switch (choice) { case 1: startGame(name); break; case 2: instructions(); break; case 3: showHighScores(); break; case 4: credits(); break; case 5: puts("\nThe BrainVault awaits your return. Goodbye!"); return 0; }
    }
}
