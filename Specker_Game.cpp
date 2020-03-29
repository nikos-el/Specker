#include <iostream>
#include <stdexcept>
#include <string>

class Move {
  public:
    Move(int sh, int sc, int th, int tc): source(sh), source_coins(sc), target(th), target_coins(tc) {}

    int getSource() const {
      return source;
    }
    int getSourceCoins() const {
      return source_coins;
    }
    int getTarget() const {
      return target;
    }
    int getTargetCoins() const {
      return target_coins;
    }

    friend std::ostream & operator << (std::ostream &out, const Move &move){
      out<<"takes "<<move.source_coins<<" coins from heap "<<move.source<<" and puts ";
      if (move.target_coins>0) out<<move.target_coins<<" coins to heap "<<move.target;
      else out<<"nothing";
      return out;
    }

  private:
      int source, source_coins, target, target_coins;
};

class State {
  public:
    State (int  h, const int c[]): heaps(h), h_coins(new int[h]) {
      for (int i=0; i<h; i++) h_coins[i]=c[i];
    }
    ~State() {
      delete [] h_coins;
    }

    void next(const Move &move) throw(std::logic_error){
      if (move.getSource()<0 || move.getSource()>=heaps ||
          move.getTarget()<0 || move.getTarget()>=heaps ||
          move.getSourceCoins()<=0 || move.getTargetCoins()<0 ||
          move.getSourceCoins()>h_coins[move.getSource()] || move.getTargetCoins()>=move.getSourceCoins())
        throw std::logic_error("invalid move");
      h_coins[move.getSource()]-=move.getSourceCoins();
      if (move.getTargetCoins()>0) h_coins[move.getTarget()]+=move.getTargetCoins();
      State(heaps,h_coins);
    }
    bool winning() const {
      int i=0;
      while (h_coins[i++]==0);
      return i>heaps;
    }

    int getHeaps() const {
      return heaps;
    }
    int getCoins(int h) const throw(std::logic_error){
      if (h<0 || h>=heaps) throw std::logic_error("invalid heap");
            return h_coins[h];
    }

    friend std::ostream & operator << (std::ostream &out, const State &state){
      for (int i=0; i<state.heaps-1; i++) out<<state.h_coins[i]<<", ";
      out<<state.h_coins[state.heaps-1];
      return out;
    }

  private:
    int heaps;
    int *h_coins;
};

class Player {
  public:
    Player(const std::string &n): name(n) {}

    virtual ~Player(){
      name.clear();
    }

    virtual const std::string & getType() const = 0;
    virtual Move play(const State &s) = 0;

    friend std::ostream & operator << (std::ostream &out, const Player &player){
      out<<player.getType()<<" player "<<player.name;
      return out;
    }

  protected:
    std::string name;
};

class GreedyPlayer: public Player {
  public:
    GreedyPlayer(const std::string &n): Player(n), type("Greedy") {}
    ~GreedyPlayer(){
      name.clear();
      type.clear();
    }

    const std::string & getType() const {
      return type;
    }
    Move play(const State &s) {
      int i_max=0, max_coins=0;
      for (int i=0; i<s.getHeaps(); i++){
        int x=s.getCoins(i);
        if (x>max_coins){
          i_max=i;
          max_coins=x;
        }
      }
      return Move(i_max, max_coins, 0, 0);
    }
  private:
    std::string type;
};

class SpartanPlayer: public Player {
  public:
    SpartanPlayer(const std::string &n): Player(n), type("Spartan") {}
    ~SpartanPlayer(){
      name.clear();
      type.clear();
    }

    const std::string & getType() const {
      return type;
    }
    Move play(const State &s) {
      int i_max=0, max_coins=0;
      for (int i=0; i<s.getHeaps(); i++){
        int x=s.getCoins(i);
        if (x>max_coins){
          i_max=i;
          max_coins=x;
        }
      }
      return Move(i_max, 1, 0, 0);
    }
  private:
    std::string type;
};

class SneakyPlayer: public Player {
  public:
    SneakyPlayer(const std::string &n): Player(n), type("Sneaky") {}
    ~SneakyPlayer(){
      name.clear();
      type.clear();
    }

    const std::string & getType() const {
      return type;
    }
    Move play(const State &s) {
      int i_min=0, min_coins=0, i=0;
      while (min_coins==0) min_coins=s.getCoins(i++);
      i_min=i-1;
      for (i=0; i<s.getHeaps(); i++){
        int x=s.getCoins(i);
        if (x>0 && x<min_coins){
          i_min=i;
          min_coins=x;
        }
      }
      return Move(i_min, min_coins, 0, 0);
    }
  private:
    std::string type;
};

class RighteousPlayer: public Player {
  public:
    RighteousPlayer(const std::string &n): Player(n), type("Righteous") {}
    ~RighteousPlayer(){
      name.clear();
      type.clear();
    }
    const std::string & getType() const {
      return type;
    }
    Move play(const State &s) {
      int i_min=0, i_max=0, min_coins=0, max_coins=0, i=0;
      while (min_coins==0) min_coins=s.getCoins(i++);
      i_min=i-1;
      for (i=0; i<s.getHeaps(); i++){
        int x=s.getCoins(i);
        if (x>max_coins){
          i_max=i;
          max_coins=x;
        }
        if (x<min_coins){
          i_min=i;
          min_coins=x;
        }
      }
      int k=(max_coins+1)/2;
      return Move(i_max, k, i_min, k-1);
    }
  private:
    std::string type;
};

class Game {
  public:
    Game(int x, int y): h(x), p(y), count_h(0), count_p(0),
                        heaps(new int[x]), players(new Player*[y]) {}
    ~Game (){
      delete [] heaps;
      delete [] players;
    }

    void addHeap(int coins) throw(std::logic_error){
      if (coins<0 || count_h>=h) throw std::logic_error("invalid heap");
      heaps[count_h]=coins;
      count_h++;
    }
    void addPlayer(Player *player) throw(std::logic_error){
      if (count_p>=p) throw std::logic_error("invalid player");
      players[count_p]=player;
      count_p++;
    }
    void play(std::ostream &out) throw(std::logic_error){
      if (h!=count_h || p!=count_p) throw std::logic_error("invalid game");
      State s(h, heaps);
      int i=0;
      while (!s.winning()){
        out<<"State: "<<s<<std::endl;
        out<<*players[i%p]<<" "<<players[i%p]->play(s)<<std::endl;
        s.next(players[i%p]->play(s));
        i++;
      }
      out<<"State: "<<s<<std::endl;
      out<<*players[(i-1)%p]<<" wins"<<std::endl;
    }

  private:
    int h, p, count_h, count_p, *heaps;
    Player **players;
};

int main(){
  Game specker(3,4);
  specker.addHeap(10);
  specker.addHeap(20);
  specker.addHeap(17);
  specker.addPlayer(new SneakyPlayer("Tom"));
  specker.addPlayer(new SpartanPlayer("Mary"));
  specker.addPlayer(new GreedyPlayer("Alan"));
  specker.addPlayer(new RighteousPlayer("Robin"));
  specker.play(std::cout);
  return 0;
}
