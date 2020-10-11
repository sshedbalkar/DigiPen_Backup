+-----------------------------+
| Names:                      |
|   -Steven George            |
|   -Santosh Shedbalkar       |
|   -David Berghan            |
| Class: CS260                |
| Instructor: Jeff Tucker     |
| Assignment 4: Asteroids     |
+-----------------------------+

+---------+
| Index : |
+---------+
  -Configuration File Specifications
  -Build Instructions
  -Usage Instructions
  -Attempted Extra Credit Options

+------------------------------------+
| Configuration File Specifications: |
+------------------------------------+
A text file with the name "config.txt" must be located in both the main
directory and the "server" directory in the project submission in order to run
either application properly. If the file is not present in the directory, then
when run, the application will attempt to read the file, fail, and safely
terminate after having freed resources. This failure will also be output to the
console window before termination. As per the assignment instructions, the file
must begin with the IP address of the computer running the server application,
followed by the TCP port that the applications should communicate on on a new
line, followed by the UDP port that the applications should communicate on,
which must be unique from one another. Any additional data in the file after
that will not be read, but will also therefore not hurt anything.

+---------------------+
| Build Instructions: |
+---------------------+
1. Open the Microsoft Visual Studio Solution file "Game" from the directory.
2. From the Solution Explorer, right-click the "server" project and select the
   "Set as StartUp Project" option.
3. Press the Control key and the F5 key together to build and run the project.
   This will launch the server application. Under the default Warning Level 3,
   there should be no errors or warnings.
4. From the Solution Explorer, right-click the "Asteroids" project and select
   the "Set as StartUp Project" option.
5. Press the Control key and the F5 key together to build and run the project.
   This will launch the client application. Under the default Warning Level 3,
   there should be no errors or warnings.
6. The server application and client application should now both be built and
   running. A game requires a minimum of two players before it can begin, so
   at least one other client will need to connect on a different computer.

+---------------------+
| Usage Instructions: |
+---------------------+
1.  From the server application, if all necessary WinSock components loaded
    correctly, the server will set itself to listening mode for a client to
    connect automatically. No additional input is required on the server side
    until you are ready to terminate the program. An on-screen instruction will
    prompt you to input "/quit" to safely terminate the server, which you can
    do at any time, even while clients are connected.
2.  From the client application, if all necessary WinSock components loaded
    correctly and connection was established with the server, the game will put
    itself into a game lobby with all other connected clients. Clients can chat
    with one another in the game lobby until a game begins.
3.  When at least two players and less than or equal to four players are
    connected, any connected player can input the string "/asteroids" to begin
    a game of asteroids between all players.
4.  From the server lobby, entering the string "/quit" will cause the program
    to free resources and close safely. Closing the window at any time will
    also cause this to happen.
   
+---------------------------------+
| Attempted Extra Credit Options: |
+---------------------------------+
1. Lobby system with chat (5 Points)
   When players first connect to the server, if no game is currently running,
   players are put in a lobby where they can operate a chat system with every
   other client. This chat system is used to start a game once enough players
   are connected.
2. Asteroids Homing Missile (4 Points)
   Homing missiles which track asteroids can be fired with the M key.