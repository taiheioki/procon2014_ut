#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/format.hpp>

#include "PlayBoard.hpp"
#include "Problem.hpp"

namespace slide
{

void Problem::load(const boost::filesystem::path& path)
{
    std::ifstream fin(path.c_str());
    if(!fin){
        throw std::runtime_error((boost::format("cannot open %s") % path.string()).str());
    }

    std::stringstream ss;
    ss << fin.rdbuf();
    fin.close();
    parse(ss);
}

void Problem::parse(std::stringstream& ss)
{
    ss >> board.width();
    ss >> board.height();
    ss >> selectionLimit;
    ss >> selectionCost;
    ss >> swappingCost;

    if(board.width() <= 0 || board.height() <= 0 || selectionLimit <= 0 || selectionCost < 0 || swappingCost < 0){
        throw std::runtime_error("problem has invalid format");
    }
    if(board.width() > 16 || board.height() > 16){
        throw std::runtime_error("problem is too large");
    }

    rep(i, board.height()) rep(j, board.width()) {
        int x;
        if(ss.bad()){
            throw std::runtime_error("problem has invalid forma");
        }
        ss >> std::hex >> x;
        board(i,j) = x;
    }
}

boost::optional<int> Problem::check(const Answer& answer) const
{
    int cost = 0;
    int remain = selectionLimit;
    PlayBoard<Flexible> board = this->board;

    // simulate
    for(int i=0; i<answer.size(); ++i){
        const Move move = answer[i];

        if(move.isSelection){
            const Point p = move.getSelected();
            if(!p.isIn(board.height(), board.width())){
                std::cerr << "the selected point is not the valid area!" << "(" << i << "/" << answer.size() << ")" << std::endl;
                std::cerr << board << std::endl;
                return boost::none;
            }
            else if(remain == 0){
                std::cerr << "number of selection exceeded the limit!" << "(" << i << "/" << answer.size() << ")" << std::endl;
                std::cerr << board << std::endl;
                return boost::none;
            }
            board.select(p);
            cost += selectionCost;
            --remain;
        }
        else{
            if(!board.isValidMove(move.getDirection())){
                std::cerr << "invalid move!" << "(" << i << "/" << answer.size() << ")" << std::endl;
                std::cerr << board << std::endl;
                return boost::none;
            }
            board.move(move.getDirection());
            cost += swappingCost;
        }
    }

    // check
    rep(i, board.height()) rep(j, board.width()){
        if(!board.isAligned(i, j)){
            std::cerr << "the answer sequence could be interpreted correctly but is not correct!"
                      << "(" << i << "/" << answer.size() << ")" << std::endl;
            std::cerr << board << std::endl;
            return boost::none;
        }
    }

    return cost;
}

std::string Problem::toString() const
{
    std::stringstream ss;
    const int width = board.width();
    const int height = board.height();

    ss << width << " ";
    ss << height << " ";
    ss << selectionLimit << " ";
    ss << selectionCost << " ";
    ss << swappingCost << "\n";
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            unsigned char idx = board.at(y,x);
            int y_ = idx / 16;
            int x_ = idx % 16;
            ss << std::hex << y_;
            ss << std::hex << x_;
            ss << " ";
        }
        ss << "\n";
    }

    return ss.str();
}

std::ostream& operator<<(std::ostream& out, const Problem& problem)
{
    out << "H = " << problem.board.height() << '\n';
    out << "W = " << problem.board.width() << '\n';
    out << "swapping cost   = " << problem.swappingCost << '\n';
    out << "selection cost  = " << problem.selectionCost << '\n';
    out << "selection limit = " << problem.selectionLimit << '\n';
    out << problem.board;

    return out;
}

bool operator==(const Problem& lhs, const Problem& rhs)
{
   if(lhs.board.height() != rhs.board.height() ||
      lhs.board.width() != rhs.board.width() ||
      lhs.swappingCost != rhs.swappingCost ||
      lhs.selectionCost != rhs.selectionCost ||
      lhs.selectionLimit != rhs.selectionLimit){
       return false;
   }
   for(int y = 0; y < lhs.board.height(); y++){
       for(int x = 0; x < lhs.board.width(); x++){
           if(lhs.board(x,y) != rhs.board(x,y)){
               return false;
           }
       }
   }
   return true;
}

} // end of namespace slide
