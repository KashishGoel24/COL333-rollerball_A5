<script setup>
import ChessBoard from 'chessboardjs-vue3';
import {onMounted, reactive} from 'vue';

const state = reactive({
    white_address: 'localhost:8181',
    black_address: 'localhost:8182',
    thinking_time: 2,

    w_conn_btn_state: 'button-to-connect',
    w_conn_btn_text: 'Connect White Bot',
    b_conn_btn_state: 'button-to-connect',
    b_conn_btn_text: 'Connect Black Bot',
    game_btn_state: 'button-to-start',
    game_btn_text: 'Start Game',

    left_info: 'rollerball v.1.0',
    right_info: 'Rev. Sep 13, 2023',

    ws_white: null,
    ws_black: null,

    game: null
})

class Game {

    constructor() {
        this.curr_player = 'white';
        this.board = new ChessBoard('myBoard', {
            'position': {
                c1: 'wP',
                c2: 'wP',
                d1: 'wB',
                d2: 'wK',
                e1: 'wR',
                e2: 'wR',

                c6: 'bR',
                c7: 'bR',
                d6: 'bK',
                d7: 'bB',
                e6: 'bP',
                e7: 'bP'

                // f1: 'bR',
                // e1: 'bP',
                // d1: 'wK'
            }
        })

        this.player_state = {
            'white': 'uninitialized',
            'black': 'uninitialized'
        }

        this.next_player = {
            'white': 'black',
            'black': 'white'
        }

        this.move_list = []
        this.position_list = []

        this.over = false;
    }

    start() {

        if (this.over) {

            // restart
            state.w_conn_btn_text = 'Connect White Bot';
            state.w_conn_btn_state = 'button-to-connect';

            state.b_conn_btn_text = 'Connect Black Bot';
            state.b_conn_btn_state = 'button-to-connect';

            state.game_btn_text = 'Start Game';
            state.game_btn_state = 'button-to-start';

            state.game = new Game();
            return
        }
        
        if (this.player_state['white'] === 'uci_initialized' && this.player_state['black'] === 'uci_initialized') {
            this.dispatch_uci_command('white', 'isready');
            this.dispatch_uci_command('black', 'isready');
        }
        else {
            state.left_info = 'Could not start game: one or more UCI engines not initialized';
        }

    }

    do_move() {
        var player = this.curr_player;
        this.dispatch_uci_command(player, 'position startpos moves ' + this.move_list.join(' ')); 
        this.dispatch_uci_command(player, 'go'); 
        this.player_state[player] = 'thinking';
        setTimeout(() => {this.dispatch_uci_command(player, 'stop')}, state.thinking_time*1000);
    }

    process_uci_command(player, command) {
        
        console.log(player + " : " + command);
        var tokens = command.split(" ");
        if (tokens[0] === 'uciok') {
            this.player_state[player] = 'uci_initialized';
        }
        else if (tokens[0] === 'readyok') {
            this.player_state[player] = 'ready';

            if (this.player_state['white'] === 'ready' && this.player_state['black'] === 'ready') {
                state.game_btn_state = 'started';
                state.game_btn_text = 'Game in progress';
                this.do_move();
            }
        }
        else if (tokens[0] === 'info') {
            state.left_info = player + ":" + tokens.slice(1).join(' ');
        }
        else if (tokens[0] === 'bestmove') {
            if (this.curr_player === player) {
                // do move
                this.player_state[this.curr_player] = 'ready';
                var move = tokens[1];
                if (move.length > 4 && move[4] !== '+') {
                    // promote - this is harder than it needs to be
                    var curr_pos = this.board.position();
                    delete curr_pos[move.slice(0,2)];
                    curr_pos[move.slice(2,4)] = this.curr_player[0] + move[4].toUpperCase();
                    this.board.position(curr_pos, true);
                    console.log(curr_pos);
                }
                else {
                    var textmove = move.slice(0,2)+"-"+move.slice(2,4);
                    this.board.move(textmove);
                    console.log('Moved ' + textmove);
                }
                this.curr_player = this.next_player[player];

                this.move_list.push(move);
                this.position_list.push(this.board.fen());

                if (move.endsWith('#')) {
                    state.left_info = player + " wins by Checkmate!";
                    this.stop();
                    return;
                }
                else if (move.endsWith('-')) {
                    // Stalemate
                    state.left_info = "Game drawn by Stalemate";
                    this.stop();
                    return;
                }
                else if (this.move_list.length >= 200) {
                    // threefold repetition
                    state.left_info = "Game truncated at 100 moves";
                    this.stop();
                }
                else if (this.position_list.filter(x => x === this.board.fen()).length >= 3) {
                    // threefold repetition
                    state.left_info = "Game drawn by threefold repetition";
                    this.stop();
                }
                else if (this.player_state[this.curr_player] === 'ready') {
                    this.do_move();
                }
            }
            else {
                state.left_info = 'ERROR: received an out-of-place move from ' + player;
            }
        }
    }

    dispatch_uci_command(player, command) {
        
        if (player === 'white') {
            state.ws_white.send(command);
        }
        else if (player === 'black') {
            state.ws_black.send(command);
        }

    }

    stop() {
        this.dispatch_uci_command('white', 'quit');
        this.dispatch_uci_command('black', 'quit');

        state.ws_white.close();
        state.ws_black.close();

        state.game_btn_state = 'button-to-start';
        state.game_btn_text = 'Reset';

        this.over = true;
    }
}

onMounted(() => {
    state.game = new Game();
});

function connect_white() {
    console.log("Connecting to white\n");

    state.w_conn_btn_state = 'button-connecting';
    state.w_conn_btn_text = 'Connecting White Bot';
    state.ws_white = new WebSocket(`ws://${state.white_address}`);

    state.ws_white.onopen = (event) => {
        state.ws_white.send("uci");
        state.w_conn_btn_state = 'button-connected';
        state.w_conn_btn_text = 'Connected to White Bot';
        console.log("Connected\n");
    }

    state.ws_white.onerror = (err) => {
        state.right_info = 'Could not connect to white bot';
        state.w_conn_btn_state = 'button-to-connect';
        state.w_conn_btn_text = 'Connect White Bot';
    }
    
    state.ws_white.onmessage = (msg) => {
        state.game.process_uci_command('white', msg.data);
    }

}

function connect_black() {
    console.log("Connecting to black\n");

    state.b_conn_btn_state = 'button-connecting';
    state.b_conn_btn_text = 'Connecting Black Bot';
    state.ws_black = new WebSocket(`ws://${state.black_address}`);

    state.ws_black.onopen = (event) => {
        state.ws_black.send("uci");
        state.b_conn_btn_state = 'button-connected';
        state.b_conn_btn_text = 'Connected to Black Bot';
        console.log("Connected\n");
    }

    state.ws_black.onerror = (err) => {
        state.right_info = 'Could not connect to black bot';
        state.b_conn_btn_state = 'button-to-connect';
        state.b_conn_btn_text = 'Connect Black Bot';
    }

    state.ws_black.onmessage = (msg) => {
        state.game.process_uci_command('black', msg.data);
    }
}

</script>

<template>

<form>
    <div class='field-pad'>
        <label for="white_address">White Address:</label>
        <input id="white_address" v-model.number="state.white_address" placeholder="white address">
    </div>
    <div class='field-pad'>
        <label for="black_address">Black Address:</label>
        <input id="black_address" v-model.number="state.black_address" placeholder="black address">
    </div>
    <div class='field-pad'>
        <label for="thinking_time">Thinking time:</label>
        <input type="number" id="thinking_time" v-model.number="state.thinking_time" placeholder="thinking time">
    </div>
</form>
<div class='button-bar'>
    <div class='button-enclose'><button :class='state.w_conn_btn_state' @click="connect_white" v-html="state.w_conn_btn_text"></button></div>
    <div class='button-enclose'><button :class='state.b_conn_btn_state' @click="connect_black" v-html="state.b_conn_btn_text"></button></div>
    <div class='button-enclose'><button :class='state.game_btn_state' @click="state.game.start" v-html="state.game_btn_text"></button></div>
</div>

<div class='board-enclose'>
<div id="myBoard" style="width: 400px"></div>
</div>

<div class='info-bar'>
    <label class='left-info' v-html="state.left_info"></label>
    <label class='right-info' v-html="state.right_info"></label>
</div>
</template>

<style scoped>
form {
  display: flex;
  width: 60%;
  padding-bottom: 20px;
}

div.button-bar {
  display: flex;
  flex-direction: row;
  width: 60%;
}

div.book-container {
  display: flex;
  flex-direction: column;
  flex-grow: 1;
}

div.button-enclose {
  flex-grow: 1;
  padding-right: 10px;
  padding-left: 10px;
  text-align: center;
}

button.button-connected,button.started {
  background-color: ForestGreen;
}

button.button-to-start,button.button-to-connect {
  background-color: GoldenRod;
}

button.button-connecting {
  background-color: Crimson;
}

button:hover {
  filter: brightness(90%);
}

button:active {
  filter: brightness(80%);
}

button {
  width: 200px;
  min-width: 150px;
  height: 30px;
  border: none;
  font-weight: bold;
  background-color: green;
  color: var(--vt-c-white);
  border-radius: 4px;
}

div.field-pad {
  width: 100%;
  padding-right: 8px;
  padding-left: 8px;
  align: center;
  text-align: center;
}

div.info-bar {
  width: 60%;
  display: flex;
  flex-direction: row;
}

label.left-info {
  text-align: left;
  flex-grow: 1;
}

label.right-info {
  text-align: right;
  flex-grow: 1;
}

label {
  padding-right: 4px;
}

input[type=number] {
  width: 60px;
}

input {
  width: 150px;
  padding: 4px;
  font-family: monospace;
  background-color: var(--vt-c-black);
  border: none;
  text-align: center;
  border-bottom: solid 2px var(--vt-c-divider-dark-1);
  color: var(--vt-c-white);
  -moz-appearance: textfield;
}

div.board-enclose {

  padding-left: 50px;
  padding-bottom: 20px;
}

#myBoard {
  clip-path: polygon(
      0px 65px, 
      15px 50px, 
      100px 50px, /* */
      100px 300px,
      250px 300px,
      250px 150px,
      100px 150px,
      100px 50px, /* */
      335px 50px, 
      350px 65px, 
      350px 385px, 
      335px 400px, 
      15px 400px, 
      0px 385px
  );
}
</style>
