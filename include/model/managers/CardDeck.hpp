#pragma once

#include <cstdlib>
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

    void addCard(shared_ptr<T> card) {
        drawPile.push_back(card);
    }
    shared_ptr<T> drawCard() {
        if(drawPile.empty()) {
            reshuffleDiscardIntoDrawPile();
        }
        shared_ptr<T> card = drawPile.back();
        drawPile.pop_back();
        return card;
    }
    void discardCard(shared_ptr<T> card) {
        discardPile.push_back(card);
    }

    void shuffle() {
        for (int i = (int)drawPile.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(drawPile[i], drawPile[j]);
        }
    }

    void reshuffleDiscardIntoDrawPile() {
        for (auto& card : discardPile) {
            drawPile.push_back(card);
        }
        discardPile.clear();
        shuffle();
    }

    bool isEmpty() const {
        return drawPile.empty();
    }

    int size() const {
        return (int)drawPile.size();
    }
};