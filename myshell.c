
// Prabin Lamichhane , id: 1002240919
// Course: CSE 3320 - Operating system
// Lab : Assignment -1 , Os calls , shell
// Date-2025-06-09
// Refrence : http://www.di.uevora.pt/~lmr/syscalls.html
// Youtube videos
// https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
/* Some example code and prototype -
contains many, many problems: should check for return values
(especially system calls), handle errors, not use fixed paths,
handle parameters, put comments, watch out for buffer overflows,
security problems, use environment variables, etc.
*/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILES 1024
#define NAME_LENGTH 256
#define PAGE_SIZE 5
// This program is a simple file manager that allows users to view, edit, run,
// change directories, sort files, and remove files in the current directory. It
// supports basic commands and displays files in pages, allowing navigation
// through them.
typedef struct {
  char name[NAME_LENGTH]; // Name of the file or directory
  int is_dir;             // Flag to indicate if the entry is a directory
  off_t size;             // Size of the file in bytes
  time_t mtime;           // Last modification time of the file
} Entry;

Entry entries[MAX_FILES]; // Array to hold file entries
int entry_count = 0;
int page = 0;
// Scans the current directory and populates the entries array with file
// information
void scan_directory() {
  DIR *d = opendir(".");
  struct dirent *de;
  struct stat st;

  entry_count = 0;
  while ((de = readdir(d)) && entry_count < MAX_FILES) {
    strcpy(entries[entry_count].name, de->d_name);
    entries[entry_count].is_dir = (de->d_type == DT_DIR);

    stat(de->d_name, &st);
    entries[entry_count].size = st.st_size;
    entries[entry_count].mtime = st.st_mtime;

    entry_count++;
  }
  closedir(d);
}
// Displays the entries in the current page
void display_entries() {
  int start = page * PAGE_SIZE;
  int end = start + PAGE_SIZE;
  if (end > entry_count)
    end = entry_count;

  printf("\n----- Page %d -------\n", page + 1);
  for (int i = start; i < end; i++) {
    printf("%d. [%s] %s\n", i, entries[i].is_dir ? "DIR" : "FILE",
           entries[i].name); // Display file index, type, and name
  }
  printf("--------------------------------\n");
}
// Opens the specified file in the nano text editor for editing
void edit_file(int index) {
  if (index >= 0 && index < entry_count && !entries[index].is_dir) {
    char cmd[NAME_LENGTH + 10];
    snprintf(cmd, sizeof(cmd), "nano %s", entries[index].name);
    system(cmd);
  } else {
    printf("Invalid file index.\n");
  }
}
// Runs the specified file
void run_file(int index, char *params) {
  if (index >= 0 && index < entry_count && !entries[index].is_dir) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "./%s %s", entries[index].name, params);
    system(cmd);
  } else {
    printf("Invalid file index.\n");
  }
}
// Changes the current working directory to the specified directory
void change_directory(int index) {
  if (index >= 0 && index < entry_count && entries[index].is_dir) {
    chdir(entries[index].name);
    scan_directory();
    page = 0;
  } else {
    printf("Invalid directory index.\n");
  }
}
// Removes the specified file from the current directory
void remove_file(int index) {
  if (index >= 0 && index < entry_count && !entries[index].is_dir) {
    if (remove(entries[index].name) == 0) {
      printf("%s removed.\n", entries[index].name);
    } else {
      perror("remove");
    }
    scan_directory();
  } else {
    printf("Invalid file index.\n");
  }
}
// Sorts the entries based on the specified mode (size or date)
void sort_entries(char mode) {
  for (int i = 0; i < entry_count - 1; i++) {
    for (int j = i + 1; j < entry_count; j++) {
      int swap = 0;
      if (mode == 's' && entries[i].size > entries[j].size)
        swap = 1;
      if (mode == 'd' && entries[i].mtime > entries[j].mtime)
        swap = 1;
      if (swap) {
        Entry temp = entries[i];
        entries[i] = entries[j];
        entries[j] = temp;
      }
    }
  }
}

int main() {
  char input;
  int index;
  char param[256];

  scan_directory();

  while (1) {
    time_t t = time(NULL);
    printf("\nTime: %s", ctime(&t));

    display_entries();

    printf("\nCommands:\n");
    printf("D Display Files\n");
    printf("E Edit file\n");
    printf("R Run file\n");
    printf("C Change directory\n");
    printf("S Sort directory (s = size, d = date)\n");
    printf("M Move to directory \n");
    printf("X Remove file\n");
    printf("N Next page\n");
    printf("P Previous page\n");
    printf("Q Quit\n");
    printf("Enter command: ");

    scanf(" %c", &input); // read command input

    switch (input) {
    case 'D':
    case 'd':
      display_entries();
      break;
    case 'E':
    case 'e':
      printf("Enter file index: ");
      scanf("%d", &index);
      edit_file(index);
      break;
    case 'R':
    case 'r':
      printf("Enter file index: ");
      scanf("%d", &index);
      getchar(); // consume newline
      printf("Enter function call 1 for files: ");
      fgets(param, sizeof(param), stdin);
      param[strcspn(param, "\n")] = 0; // remove newline
      run_file(index, param);
      break;
    case 'C': // Didnt works keept showing error
    case 'c':
    case 'M':
    case 'm':
      printf("Enter directory index: ");
      scanf("%d", &index);
      change_directory(index);
      break;
    case 'S':
    case 's':
      printf("Sort by (s = size, d = date): ");
      scanf(" %c", &input);
      sort_entries(input);
      break;
    case 'X':
    case 'x':
      printf("Enter file index to remove: ");
      scanf("%d", &index);
      remove_file(index);
      break;
    case 'N':
    case 'n':
      if ((page + 1) * PAGE_SIZE < entry_count)
        page++; //
      break;
    case 'P':
    case 'p':
      if (page > 0)
        page--;
      break;
    case 'Q':
    case 'q':
      exit(0);
      break;
    default:
      printf("Invalid input\n");
    }
  }

  return 0;
}
