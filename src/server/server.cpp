//////////////////////////////////////////////////////////////////////////////
// NoLifePony - Pony Card Game                                              //
// Copyright © 2014 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////

#include "server.hpp"
#include "player.hpp"
#include "manager.hpp"
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>
#include <list>
#include <algorithm>
#include <iomanip>

namespace nlp {
    namespace server {
        void run() {
            std::cout << R"rawraw(
                               ._^.   .?5?_                                    
                            .^!3|.   .nUDUO3".                                 
                         .^1SOo_._^^^?jSPUDDEj^                                
                       -($EU41?11oooooo1113PDUP!.                              
                     .?$EE$3ooooooooooooooo1TODO3.                             
                   .~5EE$noooooooooooT3jj5555TEDEj-                            
                  .!PU4nooooooooooo111nj$EDDDE$UDE3.                           
                 .!OE5oooooooo11!!!!!!1111o5EDSODDP?                           
                 (OEjooooo1!!!!!!!!!!!!!!!!!!34PDDU$.                          
                _$U5ooo1!!!!!!!!!!!!!!!!!!!!!!!5DDDP"                          
       -T5o?".  ~ESoo!!!!!!!!!!!!!!!!!!!!!!!!!!!jDDE!                          
      .$UDDUO43~!Oo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!$DUo                          
      "PDDDDDDUESj!!!!!!!!!!!ooo11!!!!!!!!!!!!!!!!OEo                          
      (ODDDDDDDDDE$!!!!!!!!!!!5UDDD!.$-.^((~(!!!!!3P(                          
      (PDDDDDDDDDDU44444S$3"!!!oUDD~1.10T .^.^DP_oT$.                          
      ~4DDUUDDDDDDDDDDPj!~^oUDESPDDED.000   ._.Un_O!                           
      _$DDU4DDDDDDDj".  .""_"ODDDDDDD^003^B? .-.#_54^                          
       TEDDPODDDUoUU!.?@0@#0ToDDDDDDDT._ .o.  ..3!oU5.                         
       "SDDUSPDDDo|..P@(.^_"ODDDDDDDDD~^.     ..?"SDO~                         
        !ODDUSSE51^ TE^3$..^-nDDDDDDDDU|-..  ....?DDUT                         
        .1EDDUP$no?.#-B00.  -.4DDDDUO44ES|_-.-_(4DDDDS.                        
         .!ODDDDO~."U_005.~. -(DDU4PUDOODDDDE444DDDDDS.                        
           (4UDDOj3^D.-. (0" .^DD4OUUDEUDUPSPUDDDDDDU5.                        
            _n4EUDU.#.~      ._DDDEPODDE4$OUDDDDDDDDP(                         
              ^!5S4|15-_    ..|DDDDDUP$SEDDDDDDDDDDU5.              -"?11?_    
                .1oj-$n^-.....PDUEUP$PUDDDDDDDDDDDU$!-.          .|5OEUDUES^   
                 ?!T5^(j"...-SDDU$SOUDDDDDDDDDDDDUS4OP$?.      .(4UDDDDDDDD4_  
                 ?!13P5(~^"oUDDDDPUDDDDDDDDDDDDDUS4UDDDE$^    ^5EDDDDDDDDDDUj. 
                .!!!134UDDDDDDDDDDDDDDDDDDDDDDDO$ODDDDDDU4" .(4UDDDDDDDDDDDDE( 
                _(!!!!n$EUEEUDDDDDDDDDDDDDDDUOSPUDDDDDDDDU4~14DDDDDDDDDDDDDDDS.
                ^|(?!!!o$4PPP44OUDDDDDDDDUO44OUUODDDDDDDDDU4$UDDDDDDDDDDDDDDDE?
                ^.(|(?nPUDDDDDUO$44444SSS4OUDDDDO4UDDDDDDDDDO$ODDDDDDDDDDDDDDUj
     -?j5T?^.    .(|(oEDDDDDDDDDESPPOOUUDDDDDDDDDUSEDDDDDDDDDUUDDDDDDDDDDDDUEO1
   .1OUDUEUE4!_   ||oODDDDDDDDDDDDDDDDDDDDDDDDDDDDUSODDDDDDDDDDDDDDDDDDDUOSo(^ 
  ^$UDDD4EDDDUP1. -!4DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDU$ODDDDDDDDDDDDDDDDU41^.    
 _$UDDDESDDDDDDE$".5UDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDU$UDDDDDDDDDDDDDDP!.       
.3UDDDUSUDDDDDDDU$nODDDDDDDDDDDDDESUDDDDDDDDDDDDDDDDDO$PUDDDDDDDDDDU$^         
"PDDDU4UDDDDDDDDDU$ODDDDDDDDDDDU4$UDDDDDDDDDDDDDDDDDDD4oTODDDDDDDUP!.          
!UDDE4UDDDDDDDDDDDE$UDDDDDDDDDE$o$UDDDDDDDDDDDDDDDDDDDU5.^oOUDDUP1^            
nDUPPUDDDDDDDDDDDDU$UDDDDDDDDE5" TODDDDDDDDDDDDDDDDDDDDP!  .~!1(-              
?4OEDDDDDDDDDDDDDDDUDDDDDDDDO3-  ~$DDDDDDDDDDDDDDDDDDDDU$.                     
 ^!SOUDDDDDDDDDDDDDDDDDDDDDP1.   .5UDDDDDDDDDDDDDDDDDDDD4(                     
   .^oPUDDDDDDDDDDDDDDDDDU4(      oODDDDDDDDDDDDDDDDDDDDU3.                    
      _o4UDDDDDDDDDDDDDDU$^       (ODDDDDDDDDDDDDDDDDDDDDS^                    
        ^n4UDDDDDDDDDDDU5^        ^4DDDDDDDDDDDDDDDDDDDDDP(                    
          _!4EDDDDDDDDU5-         .$DDDDDDDDDDDDDDDDDDDDDUT                    
            .~TPEUDDDE3.          .5UDDDDDDDDDDDDDDDDDDDDD$.                   
               ."?n3o~.            jEDDDDDDDDDDDDDDDDDDDDD4^                   
                                   3EDDDDDDDDDDDDDDDDDDDDDO(                   
                                   jEDDDDDDDDDDDDDDDDDDDDDU1                   
)rawraw" << std::endl;
            manager manage{};
            manage.listen<player>(273);
            for (;;) {
                manage.update();
            }
        }
    }
}
