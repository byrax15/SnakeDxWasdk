module;
module SnakeGame:GameScheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {

namespace {
    using namespace DirectX;
}

GridSquare GridSquare::MakeHead()
{
    return GridSquare {
        { 0, 0, 0, 0 },
        { .1f, .9f, .1f, 1.f },
    };
}

GridSquare GridSquare::MakeTail(XMINT4 position)
{
    return GridSquare {
        position,
        Random<XMFLOAT4>({ .2f, 1.f, .2f, 1.f }, { .4f, 1.f, .4f, 1.f }),
    };
}

GridSquare GridSquare::MakeApple()
{
    return GridSquare {
        Random<XMINT4>(GameScheduler::BOUNDS.first, GameScheduler::BOUNDS.second),
        Random<XMFLOAT4>({ 1, 0, 0, 1 }, { 1, .4f, 0, 1 }),
    };
}

GameScheduler::GameScheduler()
{
    head = GridSquare::MakeHead();
    apples.emplace_back(GridSquare::MakeApple());
}

GameScheduler::~GameScheduler()
    = default;

void GameScheduler::StepFixed()
{
    if (state == GameState::RESETTING) {
        *this = GameScheduler {};
    }

    if (state != GameState::RUNNING)
        return;

    const auto head_prev_pos = XMLoadSInt4(&head.position);

    // interpret input
    switch (direction) {
    case Direction::UP:
        head.position.y += 1;
        break;
    case Direction::DOWN:
        head.position.y -= 1;
        break;
    case Direction::LEFT:
        head.position.x -= 1;
        break;
    case Direction::RIGHT:
        head.position.x += 1;
        break;
    }

    // Move head
    for (auto [c, l, u] : std::views::zip(
             std::span { &head.position.x, 2 },
             std::span { &BOUNDS.first.x, 2 },
             std::span { &BOUNDS.second.x, 2 })) {
        if (c < l) {
            c = u;
        } else if (u < c) {
            c = l;
        }
    }

    const auto head_pos = XMLoadSInt4(&head.position);

    // Check Tail collisions
    std::atomic touched_tail = false;
    std::for_each(std::execution::par, tail.begin(), tail.end(), [&](auto&& t) {
        if (XMVector4Equal(head_pos, XMLoadSInt4(&t.position))) {
            touched_tail = true;
        }
    });
    if (touched_tail) {
        if (!tail.empty()) { // Move is skipped when apple is eaten on the frame
            for (auto [l, r] : tail | std::views::pairwise) {
                l.position = r.position;
            }
            XMStoreSInt4(&tail.rbegin()->position, head_prev_pos);
        }
        state = GameState::LOST;
        return;
    }

    // Check apple collisions
    std::vector delete_apples(apples.size(), false);
    std::atomic ate_apple = false;
    auto iter = std::views::zip(apples, delete_apples);
    std::for_each(std::execution::par, iter.begin(), iter.end(), [&](auto&& tuple) {
        auto& [apple, del] = tuple;
        if (XMVector4EqualInt(head_pos, XMLoadSInt4(&apple.position))) {
            del = true;
            ate_apple = true;
        }
    });

    for (auto [i, del] : delete_apples | std::views::enumerate | std::views::reverse) {
        if (del) {
            apples.erase(apples.begin() + i);
        }
    }

    if (ate_apple) {
        apples.emplace_back(GridSquare::MakeApple());

        const auto& last_pos = tail.rbegin() == tail.rend() ? head.position : tail.rbegin()->position;
        auto& new_tail = tail.emplace_back(GridSquare::MakeTail(last_pos));
        XMStoreSInt4(&new_tail.position, head_prev_pos);
    } else if (!tail.empty()) { // Move is skipped when apple is eaten on the frame
        for (auto [l, r] : tail | std::views::pairwise) {
            l.position = r.position;
        }
        XMStoreSInt4(&tail.rbegin()->position, head_prev_pos);
    }

    if (apples.size() < 5) {
        static std::random_device g;
        if (std::uniform_int_distribution<>(0, 100)(g) == 0) {
            apples.emplace_back(GridSquare::MakeApple());
        }
    }
}

std::vector<GridSquare> GameScheduler::Squares() const
{
    std::vector<GridSquare> squares(1 + apples.size() + tail.size());
    squares[0] = head;
    std::ranges::copy(apples, squares.begin() + 1);
    std::ranges::copy(tail, squares.begin() + 1 + apples.size());
    return squares;
}

size_t GameScheduler::SizeSquares() const
{
    return 1 + tail.size() + apples.size();
}

size_t GameScheduler::SizeBytesSquares() const
{
    return sizeof(GridSquare) * SizeSquares();
}
}
