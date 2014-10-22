#ifndef SLIDE_BRANCH_HPP_
#define SLIDE_BRANCH_HPP_

#include <type_traits>
#include <utility>

#include "PlayBoard.hpp"
#include "util/define.hpp"

namespace slide
{

template<typename Callee>
class SolveCaller
{
private:
    Callee& callee;
public:
    constexpr SolveCaller(Callee& callee) : callee(callee) {}

    template<typename... Args>
    auto operator()(Args&& ...args) const -> decltype(callee.solve(std::forward<Args>(args)...)) {
        return callee.solve(std::forward<Args>(args)...);
    }
};

template<typename Callee>
inline SolveCaller<Callee> make_solve_caller(Callee& callee)
{
    return SolveCaller<Callee>(callee);
}

#ifdef USE_FAST_BRANCH

template<typename F, typename... Args>
typename std::result_of<F(const PlayBoard<Flexible>&, Args&&...)>::type branch(F&& f, const PlayBoard<Flexible>& board, Args&& ...args)
{
    if(board.height() == 8 && board.width() == 8){
        return f(PlayBoard<8, 8>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 12 && board.width() == 8){
        return f(PlayBoard<12, 8>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 4 && board.width() == 4){
        return f(PlayBoard<4, 4>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 16 && board.width() == 16){
        return f(PlayBoard<16, 16>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 9 && board.width() == 9){
        return f(PlayBoard<9, 9>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 10 && board.width() == 10){
        return f(PlayBoard<10, 10>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 6 && board.width() == 6){
        return f(PlayBoard<6, 6>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 5 && board.width() == 5){
        return f(PlayBoard<5, 5>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 8 && board.width() == 4){
        return f(PlayBoard<8, 4>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 7 && board.width() == 7){
        return f(PlayBoard<7, 7>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 3 && board.width() == 6){
        return f(PlayBoard<3, 6>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 9 && board.width() == 8){
        return f(PlayBoard<9, 8>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 2 && board.width() == 16){
        return f(PlayBoard<2, 16>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 3 && board.width() == 16){
        return f(PlayBoard<3, 16>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 4 && board.width() == 16){
        return f(PlayBoard<4, 16>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 10 && board.width() == 8){
        return f(PlayBoard<10, 8>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 12 && board.width() == 8){
        return f(PlayBoard<12, 8>(board), std::forward<Args>(args)...);
    }
    else if(board.height() == 12 && board.width() == 12){
        return f(PlayBoard<12, 12>(board), std::forward<Args>(args)...);
    }
    else{
        return f(board, std::forward<Args>(args)...);
    }
}

#else

template<typename F, typename... Args>
typename std::result_of<F(const PlayBoard<Flexible>&, Args&&...)>::type branch(F&& f, const PlayBoard<Flexible>& board, Args&& ...args)
{
    const int x = board.height();
    if(x < 9){
        if(x < 6){
            if(x < 4){
                if(x == 2){
                    return branch<F, 2, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
                else{
                    return branch<F, 3, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
            }
            else{
                if(x == 4){
                    return branch<F, 4, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
                else{
                    return branch<F, 5, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
            }
        }
        else{
            if(x == 6){
                return branch<F, 6, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
            }
            else if(x == 7){
                return branch<F, 7, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
            }
            else{
                return branch<F, 8, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
            }
        }
    }
    else{
        if(x < 13){
            if(x < 11){
                if(x == 9){
                    return branch<F, 9, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
                else{
                    return branch<F, 10, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
            }
            else{
                if(x == 11){
                    return branch<F, 11, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
                else{
                    return branch<F, 12, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
            }
        }
        else{
            if(x < 15){
                if(x == 13){
                    return branch<F, 13, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
                else{
                    return branch<F, 14, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
            }
            else{
                if(x == 15){
                    return branch<F, 15, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
                else{
                    return branch<F, 16, Args...>(std::forward<F>(f), board, std::forward<Args>(args)...);
                }
            }
        }
    }
}

template<typename F, int H, typename... Args>
typename std::result_of<F(const PlayBoard<Flexible>&, Args&&...)>::type branch(F&& f, const PlayBoard<Flexible>& board, Args&& ...args)
{
    const int x = board.width();
    if(x < 9){
        if(x < 6){
            if(x < 4){
                if(x == 2){
                    return f(PlayBoard<H, 2>(board), std::forward<Args>(args)...);
                }
                else{
                    return f(PlayBoard<H, 3>(board), std::forward<Args>(args)...);
                }
            }
            else{
                if(x == 4){
                    return f(PlayBoard<H, 4>(board), std::forward<Args>(args)...);
                }
                else{
                    return f(PlayBoard<H, 5>(board), std::forward<Args>(args)...);
                }
            }
        }
        else{
            if(x == 6){
                return f(PlayBoard<H, 6>(board), std::forward<Args>(args)...);
            }
            else if(x == 7){
                return f(PlayBoard<H, 7>(board), std::forward<Args>(args)...);
            }
            else{
                return f(PlayBoard<H, 8>(board), std::forward<Args>(args)...);
            }
        }
    }
    else{
        if(x < 13){
            if(x < 11){
                if(x == 9){
                    return f(PlayBoard<H, 9>(board), std::forward<Args>(args)...);
                }
                else{
                    return f(PlayBoard<H, 10>(board), std::forward<Args>(args)...);
                }
            }
            else{
                if(x == 11){
                    return f(PlayBoard<H, 11>(board), std::forward<Args>(args)...);
                }
                else{
                    return f(PlayBoard<H, 12>(board), std::forward<Args>(args)...);
                }
            }
        }
        else{
            if(x < 15){
                if(x == 13){
                    return f(PlayBoard<H, 13>(board), std::forward<Args>(args)...);
                }
                else{
                    return f(PlayBoard<H, 14>(board), std::forward<Args>(args)...);
                }
            }
            else{
                if(x == 15){
                    return f(PlayBoard<H, 15>(board), std::forward<Args>(args)...);
                }
                else{
                    return f(PlayBoard<H, 16>(board), std::forward<Args>(args)...);
                }
            }
        }
    }
}

#endif

};

#endif
