import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';

function Square(props) {
    return (
      <button className="square" onClick={props.onClick}>
        {props.value}
      </button>
    );
}
  
class Board extends React.Component {
    renderSquare(i) {
      return (
        <Square
          value={this.props.squares[i]}
          onClick={() => this.props.onClick(i)}
        />
        );
    }   
  
    render() {
        return (
            <div>
                <div className="board-row">
                    {this.renderSquare(0)}
                    {this.renderSquare(1)}
                    {this.renderSquare(2)}
                </div>
                <div className="board-row">
                    {this.renderSquare(3)}
                    {this.renderSquare(4)}
                    {this.renderSquare(5)}
                </div>
                <div className="board-row">
                    {this.renderSquare(6)}
                    {this.renderSquare(7)}
                    {this.renderSquare(8)}
                </div>
            </div>
        );
    }
}
  
class Game extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            history: [
                {
                    squares: Array(9).fill(null)
                }
            ],
            stepNumber: 0,
            xIsNext: true,
            pickedpos: -1,
            isPicked: false,
            lastpos: -1,
        };
    }

    handleClick(i) {
        // store history, current board, and it's squares in temporary instances
        const history = this.state.history.slice(0, this.state.stepNumber + 1);
        const current = history[history.length - 1];
        const squares = current.squares.slice();
        // determine which player's move is next
        const player = this.state.xIsNext ? "X" : "O";

        // determine if winner already
        if (calculateWinner(squares)) {
            return;
        }

        // first six moves is a tic tac toe gamne
        if (this.state.stepNumber < 6){
            // if space is free
            if(!squares[i]){
                // set player mark in tile
                squares[i] = player;
                //add move to history and store steps, change move, and store last position
                this.setState({
                    history: history.concat([
                        {
                        squares: squares
                        }
                    ]),
                    stepNumber: history.length,
                    xIsNext: !this.state.xIsNext,
                    lastpos: i,
                });
            }
        // confer to chorus lapilli rules
        } else {
            // check if center is occupied by player
            if (squares[4] === player){
                // if occupied, change player move to pickup center
                i = 4;
            }

            // if picked up position is the player's tile and a mark hasn't already been picked up yet
            if (squares[i] === player && !this.state.isPicked){
                // set tile to null
                squares[i] = null;
                // update history array by setting tile space to empty
                let updatedHistory = history;
                updatedHistory.pop();
                updatedHistory.push({squares: squares});
                //change the state to reflect changes
                this.setState({
                    history: updatedHistory,
                    pickedpos: i,
                    isPicked: true,
                    lastpos: i,
                });
        } else {
            // allow change of move if a mark is picked up, the move isn't in the same position you picked up from, and if square is empty
            if (this.state.pickedpos !== -1 && this.state.lastpos !== i && !squares[i]){
                // determine if position placed is adjacent to picked up position
                if(isAdj(this.state.pickedpos,i)){
                    // if true set the position to player's mark
                    squares[i] = player;
                    //update state to reflect changes
                    this.setState({
                        history: history.concat([
                            {
                            squares: squares
                            }
                        ]),
                        stepNumber: history.length,
                        xIsNext: !this.state.xIsNext,
                        pickedpos: -1,
                        isPicked: false,
                        lastpos: i,
                        });
                    }
                }
            }
        }
    }
  
    render() {
        const history = this.state.history;
        const current = history[this.state.stepNumber];
        const winner = calculateWinner(current.squares);

        let status;
        if (winner) {
            status = "Winner: " + winner;
        } else {
            status = "Next player: " + (this.state.xIsNext ? "X" : "O");
        } 
  
        return (
            <div className="game">
                <div className="game-board">
                    <Board
                        squares={current.squares}
                        onClick={i => this.handleClick(i)}
                    />
                </div>
            <div className="game-info">
            <div>{status}</div></div>
            </div>
        );
    }
}
  
// ========================================
  
function calculateWinner(squares) {
    const lines = [
        [0, 1, 2],
        [3, 4, 5],
        [6, 7, 8],
        [0, 3, 6],
        [1, 4, 7],
        [2, 5, 8],
        [0, 4, 8],
        [2, 4, 6]
    ];
    for (let i = 0; i < lines.length; i++) {
        const [a, b, c] = lines[i];
        if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
            return squares[a];
        }
    }
    return null;
}

function isAdj(pickedpos, placedpos) {
    // store all adjacent values for each position 0-8
    const adj = [
        [1, 3, 4],
        [0, 2, 3, 4, 5],
        [1, 4, 5],
        [0, 1, 4, 6, 7],
        [0, 1, 2, 3, 5, 6, 7, 8],
        [1, 2, 4, 7, 8],
        [3, 4, 7],
        [3, 4, 5, 6, 8],
        [4, 5, 7],
    ];
    // if any of the positions are adjacent, return true
    const row = adj[pickedpos];
    for (const elem in row){
        if (row[elem] === placedpos) {
            return true;
        }
    }
    // if true not returned already, return false
    return false;
}

ReactDOM.render(
    <Game />, 
    document.getElementById("root")
);