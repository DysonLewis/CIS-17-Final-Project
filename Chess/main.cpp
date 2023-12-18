#include <iostream>
#include <string>
#include <map>

#include "Game.h"

#include "Player.h"


int main(int argc, const char * argv[]) {
   Game m_game = Game();

   std::map<std::string, int> columns;
   columns["a"] = 0;
   columns["b"] = 1;
   columns["c"] = 2;
   columns["d"] = 3;
   columns["e"] = 4;
   columns["f"] = 5;
   columns["g"] = 6;
   columns["h"] = 7;
   std::map<std::string, int> rows;
   rows["8"] = 0;
   rows["7"] = 1;
   rows["6"] = 2;
   rows["5"] = 3;
   rows["4"] = 4;
   rows["3"] = 5;
   rows["2"] = 6;
   rows["1"] = 7;

   std::cout << "Note enter coordinates in the form [letter_column][row_num] (eg: e4)\n";
   m_game.start();
   m_game.printBoard(); //print starting board

   while (!m_game.isFinished()) {

      Player* cur_player = m_game.getCurPlayer();
      std::string start_coord;
      std::string end_coord;

      std::cout << "Turn #: " << m_game.getTurnNum() + 1 << "| Current Player: " << cur_player->getColor() << std::endl;
      std::cout << "Piece starting coordinate? or type 'undo' to undo last move: ";
      while (getline(std::cin, start_coord) && start_coord.empty()) {
         if (start_coord.compare("undo") == 0) {
            m_game.undo();
            continue;
         }
      }
      std::cout << "Piece ending coordinate? ";
      while (getline(std::cin, end_coord) && end_coord.empty()) {
      }

      // process input
      int start_x = rows[start_coord.substr(1, 1)];
      int start_y = columns[start_coord.substr(0, 1)];

      int end_x = rows[end_coord.substr(1, 1)];
      int end_y = columns[end_coord.substr(0, 1)];

      if (m_game.isValidMove(cur_player, start_x, start_y, end_x, end_y)) {
         m_game.move(cur_player, start_x, start_y, end_x, end_y);
      } else {
         std::cout << "Invalid move!\n";
         continue;
      }
      
      m_game.storeTurnToMap();
      
      if (m_game.stalemate()){
          m_game.setFinished(); // End game if stalemate
          std::cout << "Stalemate has occurred\nThe game is over!\n";
      }
      //m_game.displayPreviousTurns();
      
      if (m_game.inCheck()) {
          std::cout << "Check!\n";
         if (m_game.inCheckMate()) {
            m_game.setFinished(); // End game if king is checkmated
            std::cout << "Checkmate!\n" << cur_player->getColor() << " wins!\n";            
            break;
         }
         std::cout << "Check!\n";
      }
      m_game.switchTurn();
   }

   return 0;
}