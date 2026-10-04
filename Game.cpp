#include "Game.h"
#include "../../HEADERS/timing/theChernoTimer.h"

#define COLOR(x) x.red, x.green, x.blue

U32 Game::squareColors[SQUARES_NUM];
const char* Game::PIECE_TEXTURES[COLORS][PIECES];

Game::Game()
{
    board.SELECTED_DEPTH = SELECTED_DEPTH;
    initSquareColors();
    initPieceTextures();
    loadTextures();
    forceMoveFont.loadFromFile("yellow_style/Yellow_Style.ttf");
    forceMoveText = { "move", forceMoveFont, 30 };
    playToEndText = { "play to end", forceMoveFont, 30 };
    playAgainText = { "play again", forceMoveFont, 30 };
    flipBoardText = { "flip", forceMoveFont, 27 };
    plusText = { "+", forceMoveFont, 30 };
    minusText = { "-", forceMoveFont, 30 };
    undoText = { "undo", forceMoveFont, 30 };
    
    searchPliesText = { std::to_string(SELECTED_DEPTH), forceMoveFont, 30 };
    RectProperties_s props
    {
        sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE),
        sf::Color::Black,
        sf::Color::Color(wSqCol.red, wSqCol.green, wSqCol.blue),
        1.0f
    };
    sf::Color altColor{ bSqCol.red, bSqCol.green, bSqCol.blue };
    createBoard(props, START_POS, ROW_ELS, altColor);
    updateBoard();
    reverseBoard();
}
void Game::reverseBoard()
{
    sf::RectangleShape temp{ rects[0] };
    for (int i{ 0 }; i < SQUARES_NUM/2; ++i)
    {
        temp = rects[i];
        rects[i] = rects[SQUARES_NUM - 1 - i];
        rects[SQUARES_NUM - 1 - i] = temp;
    }
}
void Game::initPieceTextures()
{
    for (U32 i{ wP }; i <= bK; ++i)
    {
        PIECE_TEXTURES[WHITE][i] = W_SQ_PIECE_TEXTURES[i];
        PIECE_TEXTURES[BLACK][i] = B_SQ_PIECE_TEXTURES[i];
    }
}
void Game::drawBoard()
{
    searchPliesText.setString(std::to_string(SELECTED_DEPTH));
    window.draw(forceMove);
    window.draw(forceMoveText);
    window.draw(playToEnd);
    window.draw(playToEndText);
    window.draw(playAgain);
    window.draw(playAgainText);
    window.draw(flipBoard);
    window.draw(flipBoardText);
    window.draw(increasePly);
    window.draw(plusText);
    window.draw(decreasePly);
    window.draw(minusText);
    window.draw(searchPlies);
    window.draw(searchPliesText);
    window.draw(undo);
    window.draw(undoText);
    window.draw(prev);
    window.draw(next);

    computerThinking.setTexture((playerTurn ? nullptr : &computerThinkingTexture));
    //computerThinking.setTexture(&computerThinkingTexture);
    window.draw(computerThinking);

    sideToMove.setFillColor((board.wturn ? sf::Color::White : sf::Color::Black));
    window.draw(sideToMove);
    for (int i{ 0 }; i < SQUARES_NUM; ++i)
    {
        window.draw(rects[i]);
    }
}
void Game::loadTextures()
{
    if (!computerThinkingTexture.loadFromFile("computerThinkingTexture.png")) {
        std::cout << "computerThinkingTexture.png failed to load\n";
        abort();
    }
    if (!prevTexture.loadFromFile("previous.png")) {
        std::cout << "prev.png failed to load\n";
        abort();
    }
    if (!nextTexture.loadFromFile("next.png")) {
        std::cout << "next.png failed to load\n";
        abort();
    }
    for (U32 i{ wP }; i <= bK; ++i)
    {
        (pieces[WHITE][i].loadFromFile(W_SQ_PIECE_TEXTURES[i]) && "couldn't load texture");
        (pieces[BLACK][i].loadFromFile(B_SQ_PIECE_TEXTURES[i]) && "couldn't load texture");
    }
}

bool Game::clicked(const sf::RectangleShape& shape, const sf::Vector2i& mousePos)
{
    return (mousePos.x > shape.getPosition().x
        && mousePos.x < shape.getPosition().x + shape.getSize().x
        && mousePos.y > shape.getPosition().y
        && mousePos.y < shape.getPosition().y + shape.getSize().y);
}

ChessMove Game::getMove()
{
    /*static int count{ 0 };
    ++count;*/
    ChessMove move{1000,1000};
    U32 index{};
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { 
            window.clear();
            drawBoard();
            if (selected)
                window.draw(*selected);
            window.display(); }
        sf::Vector2i mousePos{ sf::Mouse::getPosition(window) };
        if (clicked(forceMove, mousePos))
        {
            playerTurn = false;
            return { 1000, 1000 };
        }
        else if (clicked(playToEnd, mousePos))
        {
            onlyComputerPlays = true;
            playerTurn = false;
            return { 1000, 1000 };
        }
        else if (clicked(playAgain, mousePos))
        {
            onlyComputerPlays = false;
            playerTurn = true;
            previous = { 1000,1000 };
            board = Board();
            board.SELECTED_DEPTH = SELECTED_DEPTH;
            return{ 1000,1000 };
        }
        else if (clicked(flipBoard, mousePos))
        {
            reverseBoard();
            return { 1000,1000 };
        }
        else if (clicked(increasePly, mousePos))
        {
            ++SELECTED_DEPTH;
            return { 1000,1000 };
        }
        else if (clicked(decreasePly, mousePos))
        {
            --SELECTED_DEPTH;
            return { 1000,1000 };
        }
        else if (clicked(undo, mousePos) || clicked(prev, mousePos))
        {
            if (board.posHisSize > 1) {
                board.undoMove(move.startSquare);
                ++undoneMoves;
            }
            return { 1000,1000 };
        }
        else if (clicked(next, mousePos))
        {
            if (undoneMoves != 0)
            {
                board.redoMove();
                --undoneMoves;
            }
            return { 1000,1000 };
        }
        for (U32 i{ 0 }; i < SQUARES_NUM; ++i)
        {
            if (clicked(rects[i], mousePos)) {
                move.startSquare = i;
                //std::cout << i << ' ';
                //rects[i].setOutlineThickness(4);
                //U32 color{ !(static_cast<bool>(squareColors[i])) };
                rects[i].setOutlineThickness(4);
                rects[i].setOutlineColor(sf::Color::Magenta);
                if (selected) {
                    selected->setOutlineThickness(1);
                    selected->setOutlineColor(sf::Color::Black);
                    selected = nullptr;
                }
                index = i;
                window.clear();
                drawBoard();
                window.draw(rects[i]);
                window.display();
                break;
            }
        }
        if (move.startSquare == 1000)
            return { 1000,1000 };
        while (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) { 
            window.clear();
            drawBoard();
            window.draw(rects[index]);
            window.display();
        }
        while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { 
            window.clear();
            drawBoard();
            window.draw(rects[index]);
            window.display();
        }
        mousePos = sf::Mouse::getPosition(window);
        for (U32 i{ 0 }; i < SQUARES_NUM; ++i)
        {
            if (clicked(rects[i], mousePos)) {
                move.endSquare = i;
                //std::cout << i;
                rects[i].setOutlineThickness(4);
                rects[i].setOutlineColor(sf::Color::Magenta);
                //U32 color{ !(static_cast<bool>(squareColors[i])) };

                rects[move.startSquare].setOutlineThickness(1);
                rects[move.startSquare].setOutlineColor(sf::Color::Black);
                selected = (rects + i);
                return move;
            }
        }
    }
    return { 1000,1000 };
}

void Game::playMove(ChessMove move)
{
    static bool genMoves{ true };
#define COLOR(x) x.red, x.green, x.blue
    if (genMoves) {
        board.generateAllMoves();
        res = board.getResult();
    }
    if (board.isValidMove(move))
    {
        playerTurn = false;
        previous = move;
        board.playMove(board.playerMove);
        undoneMoves = 0;
        /*board.printBoard();
        std::cout << move.startSquare << ' ' << move.endSquare << '\n';*/
        //rects[move.startSquare].setTexture(nullptr);
        /*int color = squareColors[move.startSquare];
        rects[move.startSquare].setFillColor(sf::Color(COLOR(SQUARE_COLOR[color])));*/
        /*board.printBoard();
        std::cout << "\n\n";*/
        updateBoard();
        genMoves = true;
        board.generateAllMoves();
        res = board.getResult();
        //board.wturn = !board.wturn;
        //board.printBoard();
    }
    else
        genMoves = false;
}

void Game::clickAfterGameOver()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            window.clear();
            drawBoard();
            if (selected)
                window.draw(*selected);
            window.display();
        }
        sf::Vector2i mousePos{ sf::Mouse::getPosition(window) };
        clickPlayAgain(mousePos);
        clickUndo(mousePos);
        clickRedo(mousePos);
    }
}
void Game::clickPlayAgain(sf::Vector2i mousePos)
{
    if (clicked(playAgain, mousePos))
    {
        onlyComputerPlays = false;
        playerTurn = true;
        res = PLAYING;
        previous = { 1000,1000 };
        board = Board();
    }
}


void Game::clickUndo(sf::Vector2i mousePos)
{
    if (clicked(undo, mousePos) || clicked(prev, mousePos))
    {
        if (board.posHisSize > 1) {
            board.undoMove(board.FAKE_MOVE);
            ++undoneMoves;
        }
    }
}

void Game::clickRedo(sf::Vector2i mousePos)
{
    if (clicked(next, mousePos))
    {
        if (undoneMoves != 0)
        {
            board.redoMove();
            --undoneMoves;
        }
    }
}

void Game::play()
{
    window.setFramerateLimit(60);
    windowPos = { (int)sf::VideoMode::getDesktopMode().width / 2, (int)sf::VideoMode::getDesktopMode().height / 2 };
    
    windowPos.x -= static_cast<int>(window.getSize().x / 1.8f);
    windowPos.y -= static_cast<int>(window.getSize().y / 1.8f);
    window.setPosition(windowPos);
    
    ChessMove playerMove{ 1000,1000 };
   
    while (window.isOpen())
    {
        updateBoard();
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();
        }
        
        if (playerTurn && (res == PLAYING)) {
            playerMove = getMove();
        }
        else if(res == PLAYING) {
            playerTurn = (onlyComputerPlays? false : true);
            Timer timer{ "Timer" };
           //MoveScore_s ms{ board.computerMove(SELECTED_DEPTH) };
            MoveScore_s ms{ board.computerMoveTh(SELECTED_DEPTH) };
            //MoveScore_s ms{ board.compMove(SELECTED_DEPTH) };
            //std::cout << "\nTime spent: " << timer.elapsed() << " seconds\n";
            timer.stop();
            if (ms.move == board.FAKE_MOVE)
                res = board.getResult();
            else
                board.playMove(ms.move);
            std::cout << "Eval: " << ms.score << '\n';
            /*board._go_through_the_moves();
            std::cout << "end of moves\n";*/
        }
        if(res == PLAYING && playerMove.startSquare != 1000 && playerMove.startSquare != previous.startSquare)
            playMove(playerMove);
        if (res != PLAYING)
        {
            clickAfterGameOver();
        }
        window.clear();
        drawBoard();
        if(selected)
            window.draw(*selected);
        window.display();
    }
}

void Game::createBoard(RectProperties_s& props, const Position_s start, U32 rowEls, sf::Color altColor)
{
    float x{ start.x };
    float y{ start.y };
    forceMove.setSize(sf::Vector2f(SQUARE_SIZE * 2, SQUARE_SIZE / 2));
    forceMove.setPosition(start.x, start.y - 40);
    forceMove.setFillColor(sf::Color::Yellow);
    forceMoveText.setPosition(forceMove.getPosition().x + forceMove.getSize().x / 7.0f, forceMove.getPosition().y);
    forceMoveText.setPosition(forceMoveText.getPosition().x / 1.8f, forceMoveText.getPosition().y);
    forceMoveText.setFillColor(sf::Color::Black);

    playToEnd.setPosition(start.x + forceMove.getSize().x, forceMove.getPosition().y);
    playToEnd.setSize(forceMove.getSize());
    playToEnd.setFillColor(sf::Color::Red);
    playToEndText.setPosition(playToEnd.getPosition().x + playToEnd.getSize().x/10, playToEnd.getPosition().y);

    playAgain.setPosition(playToEnd.getPosition().x + playToEnd.getSize().x, playToEnd.getPosition().y);
    playAgain.setSize(playToEnd.getSize());
    playAgain.setFillColor(sf::Color::Green);
    playAgainText.setPosition(playAgain.getPosition().x + playAgain.getSize().x / 10, playAgain.getPosition().y);

    flipBoard.setSize({ playAgain.getSize().x / 4, playAgain.getSize().y });
    //flipBoard.setPosition(playAgain.getPosition().x + playAgain.getSize().x, playAgain.getPosition().y);
    flipBoard.setPosition(x + 8 * props.width_height.x - flipBoard.getSize().x, playAgain.getPosition().y);
    flipBoard.setFillColor(sf::Color::Blue);
    flipBoardText.setPosition(flipBoard.getPosition().x + flipBoard.getSize().x / 20, flipBoard.getPosition().y);

    sideToMove.setSize({ playAgain.getSize().x / 8, playAgain.getSize().y });
    sideToMove.setPosition(flipBoard.getPosition().x - sideToMove.getSize().x, flipBoard.getPosition().y);

    increasePly.setSize(sideToMove.getSize());
    increasePly.setPosition(sideToMove.getPosition().x - increasePly.getSize().x, sideToMove.getPosition().y);
    increasePly.setFillColor(sf::Color::Color(255, 140, 0));
    
    plusText.setPosition(increasePly.getPosition().x + increasePly.getSize().x / 4.2f, increasePly.getPosition().y);

    searchPlies.setSize(increasePly.getSize() * 2.0f);
    searchPlies.setPosition(increasePly.getPosition().x - searchPlies.getSize().x, increasePly.getPosition().y);
    searchPlies.setFillColor(sf::Color::White);
    searchPliesText.setPosition(searchPlies.getPosition().x + searchPlies.getSize().x / 3.0f, searchPlies.getPosition().y);
    searchPliesText.setFillColor(sf::Color::Red);

    decreasePly.setSize(increasePly.getSize());
    decreasePly.setPosition(searchPlies.getPosition().x - decreasePly.getSize().x, searchPlies.getPosition().y);
    decreasePly.setFillColor(sf::Color::Color(255, 140, 0));
    minusText.setPosition(decreasePly.getPosition().x + decreasePly.getSize().x / 4.2f, decreasePly.getPosition().y);

    computerThinking.setSize
    ({ decreasePly.getPosition().x - (playAgain.getPosition().x + playAgain.getSize().x), 
        playAgain.getSize().y });
    computerThinking.setPosition(playAgain.getPosition().x + playAgain.getSize().x, playAgain.getPosition().y);
    //computerThinking.setFillColor(sf::Color::Magenta);
    
    forceMove.setSize({ forceMove.getSize().x / 2, forceMove.getSize().y });
    undo.setSize(forceMove.getSize());
    undo.setPosition(forceMove.getPosition().x + forceMove.getSize().x, forceMove.getPosition().y);
    undo.setFillColor(sf::Color::Yellow);
    undo.setOutlineThickness(1);
    undo.setOutlineColor(sf::Color::Black);

    undoText.setPosition(undo.getPosition().x + undo.getSize().x / 6, undo.getPosition().y);
    undoText.setFillColor(sf::Color::Black);

    prev.setSize({ undo.getSize().x / 2, undo.getSize().y });
    prev.setPosition(start.x + SQUARE_SIZE * 3.5f, start.y + 8 * SQUARE_SIZE);
    prev.setTexture(&prevTexture);

    next.setSize(prev.getSize());
    next.setPosition(prev.getPosition().x + SQUARE_SIZE / 2, prev.getPosition().y);
    next.setTexture(&nextTexture);

    for (int i{ 0 }; i < SQUARES_NUM; ++i)
    {
        rects[i] = sf::RectangleShape{ props.width_height };
        rects[i].setOutlineColor(props.outLineColor);
        if ((i + 1) % 2)
            rects[i].setFillColor(props.fillColor);
        else
            rects[i].setFillColor(altColor);
        rects[i].setPosition(x, y);
        rects[i].setOutlineThickness(props.outLineThickness);

        if ((i + 1) % (rowEls) == 0) {
            y += rects[i].getSize().y;
            x = start.x;
            sf::Color temp{ altColor };
            altColor = props.fillColor;
            props.fillColor = temp;
        }
        else {
            x += rects[i].getSize().x;
        }
    }
}

void Game::updateBoard()
{
    U64 pos;
    U32 lsb;
    
    for (int i{ wP }; i <= bK; ++i)
    {
        pos = board.bitBoard[i];
        while (pos)
        {
            lsb = pop_LSB(pos);
            int color = squareColors[lsb];
            rects[lsb].setTexture(&pieces[color][i]);
            rects[lsb].setFillColor(sf::Color::White);
        }
    }

    pos = board.emptyBitBoard;
    while (pos)
    {
        lsb = pop_LSB(pos);
        int color = squareColors[lsb];
        rects[lsb].setFillColor(sf::Color(COLOR(SQUARE_COLOR[color])));
        rects[lsb].setTexture(nullptr);
    }
}

void Game::initSquareColors()
{
    U32 whiteColor{ WHITE };
    U32 blackColor{ BLACK };
    U32 color{ whiteColor };
    for (U32 i{ 0 }; i < SQUARES_NUM; ++i)
    {
        if ((i + 1) % 2)
            color = whiteColor;
        else
            color = blackColor;
        squareColors[i] = color;
        if ((i + 1) % (ROW_ELS) == 0)
        {
            U32 temp{ whiteColor };
            whiteColor = blackColor;
            blackColor = temp;
        }
    }
    
}