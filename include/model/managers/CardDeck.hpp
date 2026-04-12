#pragma once

#include <memory>
#include <vector>

using namespace std;

template <typename T>
class CardDeck {
private:
    vector<shared_ptr<T>> drawPile;
    vector<shared_ptr<T>> discardPile;

public:
    CardDeck() = default;
    CardDeck(const vector<shared_ptr<T>>& drawPile, const vector<shared_ptr<T>>& discardPile)
        : drawPile(drawPile), discardPile(discardPile) {}
    CardDeck(const CardDeck<T>& other) = default;
    ~CardDeck() = default;
    CardDeck<T>& operator=(const CardDeck<T>& other) = default;

    void addCard(shared_ptr<T> card) {}
    shared_ptr<T> drawCard() { return nullptr; }
    void discardCard(shared_ptr<T> card) {}
    void shuffle() {}
    void reshuffleDiscardIntoDrawPile() {}
    bool isEmpty() const { return false; }
    int size() const { return 0; }
};