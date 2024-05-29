#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <json/json.h> // Jsoncpp library

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    std::string name;
    std::cout << "Enter the name to search on Bilibili: ";
    std::getline(std::cin, name);

    std::string command = "python3 search_bilibili.py \"" + name + "\"";
    std::string result = exec(command.c_str());

    // Read the JSON output from the Python script
    std::ifstream file("bilibili_comments.json");
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string jsonContent = buffer.str();
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(jsonContent, root)) {
        std::ofstream outputFile("output.txt");
        for (const auto& video : root) {
            outputFile << "Title: " << video["title"].asString() << "\n";
            outputFile << "URL: " << video["url"].asString() << "\n";
            outputFile << "Comments:\n";
            for (const auto& comment : video["comments"]) {
                outputFile << "- " << comment.asString() << "\n";
            }
            outputFile << "\n";
        }
        outputFile.close();
        std::cout << "Comments saved to output.txt" << std::endl;
    } else {
        std::cerr << "Failed to parse JSON from Python script" << std::endl;
    }

    return 0;
}
