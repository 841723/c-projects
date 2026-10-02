#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

#define COLOR_TEXT_RED 31
#define COLOR_BG_RED 41

#define COLOR_TEXT_WHITE 37
#define COLOR_BG_WHITE 47

#define COLOR_TEXT_GREEN 32
#define COLOR_BG_GREEN 42



const char *txt = "hola yo me llamo diego raul como te llamas tu";

struct terminal_config {
    struct termios oldt, newt;
};

int count_words(const char *text) {
    char separator = ' ';
    int i = 0, count = 0, inside_word = 0, char_;

    while((char_=text[i]) != '\0') {
        if (inside_word) {
            if (char_ == separator) {
                inside_word = 0;
                count++;
            }
        }
        else {
            if (char_ != separator) inside_word = 1;
        }
        
        i++;
    }
    if (inside_word) count++;
    return count;
}

int count_chars(const char *text) {
    int count = 0;
    while (text[count++] != '\0');
    return count - 1;
}

void printfc(const char char_, int text_color, int bg_color) {
    printf("\033[%d;%dm%c",text_color, bg_color, char_);
}

void restore_print() {
    printf("\033[0m");
}

void terminal_set_up(struct terminal_config *terminal_config) {
    tcgetattr(STDIN_FILENO, &(terminal_config->oldt));
    terminal_config->newt = terminal_config->oldt;
    
    (terminal_config->newt).c_lflag &= ~(ICANON | ECHO);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &(terminal_config->newt));
}

void terminal_end(struct terminal_config *terminal_config) {
    tcsetattr(STDIN_FILENO, TCSANOW, &(terminal_config->oldt));
}

double get_elapsed_time_s(struct timeval *start, struct timeval *stop) {
    return (double)(((stop->tv_sec - start->tv_sec) * 1000000 + stop->tv_usec - start->tv_usec) / 1000) / 1000;
}

void show_screen(char *user_input, struct timeval *starttime, int correctchars) {
    struct timeval partialtime;
    
    printf("\033[2J\033[H");
    printf("###################################################\n");
    if (starttime != NULL) {
        gettimeofday(&partialtime, NULL);
        double elaspsedtime = get_elapsed_time_s(starttime, &partialtime);
        printf("Time: %.2f s\n", elaspsedtime);
    }
    else {
        printf("Time: waiting to start...\n");
    }
    printf("\n");
    
    int color = 0;

    for (int i = 0 ; i < count_chars(txt); i++) {
        color = i < correctchars ? COLOR_BG_GREEN : COLOR_BG_RED;
        printfc(txt[i], COLOR_TEXT_WHITE, color);
    }
    restore_print();
    printf("\n\n");
    printf("%s", user_input);
}

int main() {
    int read, continue_ = 1, total_keys_pressed = 0, total_letters_pressed = 0, ref_pointer = 0, usr_pointer = 0, started = 0, ref_chars = count_chars(txt);
    char user_input[1024] = "";
    struct terminal_config terminal_config;
    struct timeval stop, start;

    terminal_set_up(&terminal_config);    
    
    show_screen("", NULL, 0);

    while (continue_) {

        read = getchar();
        if (!started) {
            gettimeofday(&start, NULL);
        }
        started = 1;


        if (read == '\n') {
            continue_ = 0;
        }
        else {
            total_keys_pressed++;
            if (read == 127) {
                // delete
                if (usr_pointer > 0) --usr_pointer;
            } else {
                total_letters_pressed++;
                if (usr_pointer >= (sizeof(user_input)-1)) continue_ = 0; 
                user_input[usr_pointer++] = read;
                if (read == txt[ref_pointer]) {
                    ref_pointer++;
                    if (ref_pointer == ref_chars) continue_ = 0;
                }
            }
            user_input[usr_pointer] = '\0';
            show_screen(user_input, &start, ref_pointer);
        }

    }
    gettimeofday(&stop, NULL);
    user_input[usr_pointer] = '\0';
    double elapsed_time = get_elapsed_time_s(&start, &stop);
    int total_words = count_words(user_input);

    printf("\n\n\n");
    printf("Correct %d chars\n", ref_pointer);
    printf("Total keys pressed %d\n", total_keys_pressed);
    printf("Total words typed %d\n", total_words);
    printf("Took %.2f s\n", elapsed_time);

    double speed = total_words * 60 / elapsed_time;
    printf("\nSpeed %.2f words/min\n", speed);
    double accuracy = (double)ref_pointer / total_letters_pressed * 100;
    printf("Accuracy %.2f %%\n", accuracy);

    
    terminal_end(&terminal_config);

    return 0;
}
