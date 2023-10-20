<script setup>
import ChessBoard from 'chessboardjs-vue3';
import {onMounted, reactive} from 'vue';

const state = reactive({
    white_address: 'localhost:8181',
    black_address: 'localhost:8182',
    thinking_time: 60,

    w_conn_btn_state: 'button-to-connect',
    w_conn_btn_text: 'Connect White Bot',
    b_conn_btn_state: 'button-to-connect',
    b_conn_btn_text: 'Connect Black Bot',
    game_btn_state: 'button-to-start',
    game_btn_text: 'Start Game',

    board_class: 'board-7-3',
    board_enclose_class: 'board-enclose-7-3',

    left_info: 'rollerball v.2.0',
    right_info: 'Rev. Oct 24, 2023',

    ws_white: null,
    ws_black: null,

    game: null,
    in_progress: false,
    curr_player: 'white',

    w_time_left: 0,
    b_time_left: 0

})

const pos_7_3 = {
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
}

const pos_8_4 = {
    c1: 'wP',
    c2: 'wP',
    d1: 'wB',
    d2: 'wK',
    e1: 'wR',
    e2: 'wR',
    f1: 'wP',
    f2: 'wP',

    c7: 'bP',
    c8: 'bP',
    d7: 'bR',
    d8: 'bR',
    e7: 'bK',
    e8: 'bB',
    f7: 'bP',
    f8: 'bP'
}

const pos_8_2 = {
    c1: 'wP',
    c2: 'wP',
    c3: 'wP',
    d2: 'wN',
    d3: 'wN',
    e2: 'wK',
    e3: 'wB',
    f1: 'wR',
    f2: 'wR',
    f3: 'wP',

    c8: 'bR',
    c7: 'bR',
    c6: 'bP',
    d7: 'bK',
    d6: 'bB',
    e7: 'bN',
    e6: 'bN',
    f8: 'bP',
    f7: 'bP',
    f6: 'bP',
}

class Game {

    constructor() {
        this.curr_player = 'white';
        this.w_time_left_ms = 0;
        this.b_time_left_ms = 0;
        this.board = new ChessBoard('myBoard', {
            'position': pos_7_3,
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

        state.in_progress = true;

        this.w_time_left_ms = state.thinking_time*1000;
        this.b_time_left_ms = state.thinking_time*1000;
        state.w_time_left = state.thinking_time;
        state.b_time_left = state.thinking_time;
        this.w_timer = setInterval(() => {
            if (this.player_state['white'] == 'thinking') {
                if (this.w_time_left < 50) {
                    state.left_info = 'White ran out of time';
                    this.stop();
                }
                this.w_time_left_ms -= 50;
                var int_time_left = Math.floor(this.w_time_left_ms / 1000);
                if (int_time_left != state.w_time_left) {
                    state.w_time_left = int_time_left;
                }
            }
        }, 50);
        this.b_timer = setInterval(() => {
            if (this.player_state['black'] == 'thinking') {
                if (this.b_time_left < 50) {
                    state.left_info = 'Black ran out of time';
                    this.stop();
                }
                this.b_time_left_ms -= 50;
                var int_time_left = Math.floor(this.b_time_left_ms / 1000);
                if (int_time_left != state.b_time_left) {
                    state.b_time_left = int_time_left;
                }
            }
        }, 50);

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
            this.dispatch_uci_command('white', `ucinewgame ${state.board_class} ${this.w_time_left_ms}`);
            this.dispatch_uci_command('black', `ucinewgame ${state.board_class} ${this.b_time_left_ms}`);
        }
        else {
            state.left_info = 'Could not start game: one or more UCI engines not initialized';
        }

    }

    do_move() {
        var player = this.curr_player;
        this.dispatch_uci_command(player, 'position startpos moves ' + this.move_list.join(' ')); 
        if (player == 'white') this.dispatch_uci_command(player, `go ${this.b_time_left_ms}`); 
        else this.dispatch_uci_command(player, `go ${this.b_time_left_ms}`);
        this.player_state[player] = 'thinking';
        // setTimeout(() => {this.dispatch_uci_command(player, 'stop')}, state.thinking_time*1000);
    }

    process_uci_command(player, command) {
        
        console.log(player + " : " + command);
        var tokens = command.split(" ");
        if (tokens[0] === 'uciok') {
            this.player_state[player] = 'uci_initialized';
        }
        else if (tokens[0] === 'newgameok') {
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

function set_7_3_board() {
    state.board_class = 'board-7-3';
    state.board_enclose_class = 'board-enclose-7-3';
    state.game.board.position(pos_7_3, false);
}

function set_8_4_board() {
    state.board_class = 'board-8-4';
    state.board_enclose_class = 'board-enclose-8-4';
    state.game.board.position(pos_8_4, false);
}

function set_8_2_board() {
    state.board_class = 'board-8-2';
    state.board_enclose_class = 'board-enclose-8-2';
    state.game.board.position(pos_8_2, false);
}

</script>

<template>

<form>
    <div class='field-pad'>
        <label for="white_address">White Address:</label>
        <input id="white_address" v-model="state.white_address" placeholder="white address">
    </div>
    <div class='field-pad'>
        <label for="black_address">Black Address:</label>
        <input id="black_address" v-model="state.black_address" placeholder="black address">
    </div>
    <div class='field-pad'>
        <label for="thinking_time">Time per side:</label>
        <input type="number" id="thinking_time" v-model.number="state.thinking_time" placeholder="thinking time">
    </div>
</form>
<div class='button-bar'>
    <div class='button-enclose'><button :disabled='state.in_progress' :class='state.w_conn_btn_state' @click="connect_white" v-html="state.w_conn_btn_text"></button></div>
    <div class='button-enclose'><button :disabled='state.in_progress' :class='state.b_conn_btn_state' @click="connect_black" v-html="state.b_conn_btn_text"></button></div>
    <div class='button-enclose'><button :disabled='state.in_progress' :class='state.game_btn_state' @click="state.game.start" v-html="state.game_btn_text"></button></div>
</div>
<div class='timer'>
    <div class='time-left'>White: {{state.w_time_left}}</div>
    <div class='time-left'>Black: {{state.b_time_left}}</div>
</div>

<div :class='state.board_enclose_class'>
<div id="myBoard" :class="state.board_class" style="width: 400px"></div>
</div>

<div class='button-bar'>
    <div class='button-enclose'><button :disabled='state.in_progress' class='button-to-start' @click="set_7_3_board">7_3 board</button></div>
    <div class='button-enclose'><button :disabled='state.in_progress' class='button-to-start' @click="set_8_4_board">8_4 board</button></div>
    <div class='button-enclose'><button :disabled='state.in_progress' class='button-to-start' @click="set_8_2_board">8_2 board</button></div>
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
}

div.button-bar {
  padding-top: 20px;
  display: flex;
  flex-direction: row;
  width: 60%;
  padding-bottom: 20px;
}

div.timer {
  display: flex;
  flex-direction: row;
  width: 60%;
}

div.time-left {
    flex-grow: 1;
    text-align: center;
    font-weight: bold;
    font-size: 14pt;
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

button:disabled {

  filter: brightness(40%);

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

div.board-enclose-7-3 {

  padding-left: 50px;
  padding-bottom: 20px;
}

div.board-enclose-8-4 {

  padding-left: 0px;
  padding-bottom: 20px;
  padding-top: 20px;
}

div.board-enclose-8-2 {

  padding-left: 0px;
  padding-bottom: 20px;
  padding-top: 20px;
}

.board-7-3 {
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

.board-8-4 {
  clip-path: polygon(
      0px 15px, 
      15px 0px, 
      100px 0px, /* */
      100px 300px,
      300px 300px,
      300px 100px,
      100px 100px,
      100px 0px, /* */
      385px 0px, 
      400px 15px, 
      400px 385px, 
      385px 400px, 
      15px 400px, 
      0px 385px
  );
}

.board-8-2 {
  clip-path: polygon(
      0px 15px, 
      15px 0px, 
      150px 0px, /* */
      150px 250px,
      250px 250px,
      250px 150px,
      150px 150px,
      100px 0px, /* */
      385px 0px, 
      400px 15px, 
      400px 385px, 
      385px 400px, 
      15px 400px, 
      0px 385px
  );
}
</style>
