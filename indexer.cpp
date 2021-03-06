//
// Created by fpopic on 24.05.17.
//

#include <cstdio>
#include <cmath>
#include <chrono>
#include <unordered_map>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ClangTidyInspection"
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

using namespace std;

typedef chrono::high_resolution_clock Time;
typedef chrono::duration<float> fsec;

template<typename K, typename V>
using hmap = unordered_map<K, V>;

inline bool contains(hmap<int, int>& map, int key) { return map.find(key) != map.end(); }

void indexize_item_item(const string& input_path, hmap<int, int>& items_lookup) {
    auto start = Time::now();

    string output_path = input_path + ".indexed";
    string lookup_output_path = input_path + ".indexed.lookup";
    string lookup_size_output_path = input_path + ".indexed.lookup.size";

    FILE* input = fopen(input_path.c_str(), "r");
    FILE* output = fopen(output_path.c_str(), "w+");
    FILE* lookup_output = fopen(lookup_output_path.c_str(), "w+");
    FILE* lookup_size_output = fopen(lookup_size_output_path.c_str(), "w+");

    int counter = 0;

    int parsed, line_counter = 0;
    int itemIndex1, itemIndex2;
    int itemId1, itemId2, a, b, c, d;

    while ((parsed = fscanf(input, "%d,%d,%d,%d,%d,%d\n", &itemId1, &itemId2, &a, &b, &c, &d)) != -1) {
        if (likely(parsed == 6)) {
            itemIndex1 = contains(items_lookup, itemId1) ? items_lookup.at(itemId1) : (items_lookup[itemId1] = counter++);
            itemIndex2 = contains(items_lookup, itemId2) ? items_lookup.at(itemId2) : (items_lookup[itemId2] = counter++);
            fprintf(output, "%d,%d,%d,%d,%d,%d\n", itemIndex1, itemIndex2, a, b, c, d);
        }
        else {
            char chr;
            if (line_counter == 0) {
                printf("Header: ");
            }
            while ((chr = (char) fgetc(input)) != '\n') {
                printf("%c", chr);
                if (line_counter == 0)
                    fputc(chr, output);
            }
            printf("\n");
            if (line_counter == 0)
                fputc('\n', output);
            else
                printf("Check discarded line #%d.\n", line_counter);
        }
        line_counter++;
        if (unlikely(line_counter % 1000000 == 0)) {
            printf("Processed lines:%d\n", line_counter);
        }
    }

    printf("Output file with %d lines saved.\n", line_counter);

    for (const auto& entry : items_lookup) {
        fprintf(lookup_output, "%d,%d\n", entry.first, entry.second);
    }
    fprintf(lookup_size_output, "%d", (int) items_lookup.size());

    printf("Lookup file with %d entries saved.\n", (int) items_lookup.size());

    fsec elapsed_seconds = Time::now() - start;
    printf("Time: %ds\n", (int) floor(elapsed_seconds.count()));

    fclose(input);
    fclose(output);
    fclose(lookup_output);
    fclose(lookup_size_output);
}

void indexize_user_item(string input_path, hmap<int, int>& items_lookup) {
    auto start = Time::now();

    string output_path = input_path + ".indexed";
    string lookup_output_path = input_path + ".indexed.lookup";
    string lookup_size_output_path = input_path + ".indexed.lookup.size";

    FILE* input = fopen(input_path.c_str(), "r");
    FILE* output = fopen(output_path.c_str(), "w+");
    FILE* lookup_output = fopen(lookup_output_path.c_str(), "w+");
    FILE* lookup_size_output = fopen(lookup_size_output_path.c_str(), "w+");


    int counter = 0;
    hmap<int, int> users_lookup; // <user_id, user_index>

    int parsed, line_counter = 0;

    int userId;
    char date[10]; // dd/mm/yyyy
    int itemId;
    double quantity;

    int userIndex, itemIndex;

    while ((parsed = fscanf(input, "%d,%[^,],%d,%lf\n", &userId, &date[0], &itemId, &quantity)) != -1) {
        if (likely(parsed == 4)) {
            itemIndex = items_lookup.at(itemId); // must exist
            userIndex = contains(users_lookup, userId)
                        ? users_lookup.at(userId)
                        : (users_lookup[userId] = counter++);
            fprintf(output, "%d,%s,%d,%lf\n", userIndex, date, itemIndex, quantity);
        }
        else {
            char chr;
            if (line_counter == 0) {
                printf("Header: ");
            }
            while ((chr = (char) fgetc(input)) != '\n') {
                printf("%c", chr);
                if (line_counter == 0)
                    fputc(chr, output);
            }
            printf("\n");
            if (line_counter == 0)
                fputc('\n', output);
            else
                printf("Check discarded line #%d.\n", line_counter);
        }
        line_counter++;
        if (unlikely((line_counter % 1000000 == 0))) {
            printf("Processed lines:%d\n", line_counter);
        }
    }

    printf("Output file with %d lines saved.\n", line_counter);

    for (const auto& entry : users_lookup) {
        fprintf(lookup_output, "%d,%d\n", entry.first, entry.second);
    }
    fprintf(lookup_size_output, "%d", (int) users_lookup.size());

    printf("Lookup file with %d entries saved.\n", (int) users_lookup.size());

    fsec elapsed_seconds = Time::now() - start;
    printf("Time: %ds\n", (int) floor(elapsed_seconds.count()));

    fclose(input);
    fclose(output);
    fclose(lookup_output);
    fclose(lookup_size_output);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Wrong input args, accepting only: \"<exe> <item_item_input> <user_item_input>\"\n");
        return -1;
    }

    hmap<int, int> items_lookup; // <item_id, item_index>

    indexize_item_item(argv[1], items_lookup);

    printf("\n");

    indexize_user_item(argv[2], items_lookup);

    return 0;
}
#pragma clang diagnostic pop