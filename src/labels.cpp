
#include "labels.hpp"

Label::Label(LabelCorner _corner, std::string _format_text) {
    format_text = _format_text;
    corner = _corner;
}

std::string Label::get_text() {
    std::string result = format_text;
    result = replace_all(result, "%hui", "popa jopa hui");
    return result;
}

std::string Label::replace_all(std::string src, std::string replace, std::string replacement) {
    if (replace.empty()) return src;
    size_t start_pos = 0;
    while ((start_pos = src.find(replace, start_pos)) != std::string::npos) {
        src.replace(start_pos, replace.length(), replacement);
        start_pos += replacement.length();
    }
    return src;
}

std::string Labels::get_label_string(LabelCorner corner) {
    std::string result = "";
    
    for (Label& l : Labels::get().labels) {
        if (l.corner != corner) continue;
        if (l.format_text.empty()) continue;

        result += l.get_text();
        result += "\n";
    }
    return result;
}

void Labels::add(Label label) {
    Labels::get().labels.push_back(label);
}

void Labels::remove(int index) {
    // aciddev_: why is .begin a thing why not just .erase(index)
    //           i will never fully understand cpp...
    Labels::get().labels.erase(std::next(Labels::get().labels.begin(), index));
}

void Labels::move_up(int index) {
    if (index <= 0) return;
    if (index >= Labels::get().labels.size()) return;

    std::iter_swap(std::next(Labels::get().labels.begin(), index), std::next(Labels::get().labels.begin(), index - 1));
}

void Labels::move_down(int index) {
    if (index < 0) return;
    if (index >= Labels::get().labels.size()-1) return;

    std::iter_swap(std::next(Labels::get().labels.begin(), index), std::next(Labels::get().labels.begin(), index + 1));
}

void Labels::swap(int index_0, int index_1) {
    // if (!(0 >= index_0 && index_0 >= Labels::get().labels.size())) return;
    // if (!(0 >= index_1 && index_1 >= Labels::get().labels.size())) return;

    std::iter_swap(std::next(Labels::get().labels.begin(), index_0), std::next(Labels::get().labels.begin(), index_1));
}
