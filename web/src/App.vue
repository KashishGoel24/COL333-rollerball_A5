<script setup>
import ChessBoard from 'chessboardjs-vue3';
import Board from './components/Board.vue'
import {onMounted, reactive, watch} from 'vue';

const state = reactive({

    sockets : {
        white : {
            socket : null,
            address : 'localhost:8181',
            state : 'disconnected'
        },
        black : {
            socket : null,
            address : 'localhost:8182',
            state : 'disconnected'
        }
    },

    game : {
        type  : 'board-7-3',
        state : 'idle',
        curr_player : 'white',
        move_list : [],
        position_list : []
    },

    players : {
        white : 'waiting',
        black : 'waiting'
    },

    timer : {
        time_limit: 60,
        white : {
            time_ms : 0
        },
        black : {
            time_ms : 0
        }
    },

    info : {
        left  : 'rollerball v.2.0',
        right : 'Rev. Oct 24, 2023'
    }

});

var timer_interval = null;

const INTERVAL = 50;

function send_message(side, message) {

    if (state.sockets[side].state === 'connected') {
        state.sockets[side].socket.send(message);
    }
    else {
        stop_game(`The ${side} side disconnected arbitrarily`);
    }

}

function process_message(side, message) {

    var tokens = message.trim().split(' ');
    var command = tokens[0];

    if      (command === 'uciok')        on_uciok(tokens);
    else if (command === 'ucinewgameok') on_ucinewgameok(tokens);
    else if (command === 'bestmove')     on_bestmove(tokens);
    else {
        stop_game(`Unknown command ${command} received from ${side}`);
    }
}

function connect_socket(side) {

    state.sockets[side].state  = 'connecting';
    state.sockets[side].socket = new WebSocket(`ws://${state.sockets[side].address}`);

    state.sockets[side].onopen = (event) => {
        state.ws_white.send("uci");
    }

    state.sockets[side].onerror = (err) => {
        state.right_info = `Could not connect to ${side} bot`;
        state.sockets[side].state = 'disconnected';
    }
    
    state.sockets[side].onmessage = (msg) => {
        state.game.process_uci_command(side, msg.data);
    }
}

function disconnect_socket(side) {
    state.sockets[side].socket.close();
    state.sockets[side].socket.state = 'disconnected';
}

function tick() {

    if (state.game.state === 'white_thinking') {
        state.timer.white.time_ms -= INTERVAL;
    }
    else if (state.game.state === 'black_thinking') {
        state.timer.black.time_ms -= INTERVAL;
    }

    if (state.timer.white.time_ms <= 0) {
        stop_game('White lost by timeout');
    }
    else if (state.timer.black.time_ms <= 0) {
        stop_game('Black lost by timeout');
    }

}

function start_game() {

    timer_interval = setInterval(tick, INTERVAL);
    state.game.state = 'starting';
    send_message('black', `ucinewgame ${state.game.type} ${state.timer.time_limit}`);
    send_message('white', `ucinewgame ${state.game.type} ${state.timer.time_limit}`);

}

function stop_game(message = 'Something bad happened') {

    clearInterval(timer_interval);
    state.game.state = 'final';
    state.info.right = message;
    state.players.white = state.players.black = 'waiting';
}

function reset_game() {

    state.game.state = 'idle'
    state.game.curr_player = 'white'
    state.game.move_list = []
    state.game.position_list = []

}

function on_uciok(side, tokens) {
    state.sockets[side].state = 'connected';
    if (state.sockets.white.state === state.sockets.black.state && 
        state.sockets.white.state === 'connected') {
        state.game.state = 'ready';
    }
}

function on_ucinewgameok(side, tokens) {

    players[side] = 'ready';

    if (players.white === 'ready' && players.black === 'ready') {
        state.game.state = 'white_thinking';
        players.white = 'thinking';
        ask_for_move('white');
    }
        
}

function ask_for_move(side) {
    move_message = `position startpos moves ${state.game.move_list.join(' ')}`;
    send_message(side, move_message);
    send_message(side, `go ${state.timer.white.time_ms}`);
}

function on_bestmove(side, tokens) {

    var move = tokens[1];

    // check if it's a null move. If yes, declare checkmate/stalemate
    if (move === '0000') {
        stop_game(`${side} gave a null move, indicating either Checkmate or Stalemante`);
    }

    state.game.move_list.append(move);

    // check draw conditions - 3fold repetition (can skip for now?)

    if (side === 'white') {
        players.white = 'ready';
        players.black = 'thinking';
        state.game.state = 'black_thinking';
        ask_for_move('black');
    }
    else if (side === 'black') {
        players.white = 'thinking';
        players.black = 'ready';
        state.game.state = 'white_thinking';
        ask_for_move('white');
    }
    else {
        stop_game(`Bad side ${side} passed to on_bestmove`);
    }

}

class Game {

    constructor() {
        this.reset();
    }

    reset() {

        this.curr_player = 'white';
        this.w_time_left_ms = 0;
        this.b_time_left_ms = 0;

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
    
    start_stop() {
        if (!state.in_progress) this.start();
        else {
            this.stop();
            this.reset();
        }
    }

    start() {

        state.in_progress = true;

        this.w_time_left_ms = state.thinking_time*1000;
        this.b_time_left_ms = state.thinking_time*1000;
        state.w_time_left = state.thinking_time;
        state.b_time_left = state.thinking_time;
        this.w_timer = setInterval(() => {
            if (this.player_state['white'] == 'thinking') {
                if (this.w_time_left_ms < 50) {
                    state.left_info = 'White ran out of time';
                    // TODO wait for reset!
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
                if (this.b_time_left_ms < 50) {
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
                state.game_btn_text = 'Stop Game';
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

                this.curr_player = this.next_player[player];

                state.game_moves.push(move);
                this.move_list.push(move);
                // this.position_list.push(this.board.fen());

                if (this.move_list.length >= 200) {
                    state.left_info = "Game truncated at 100 moves";
                    this.stop();
                }
                // else if (this.position_list.filter(x => x === this.board.fen()).length >= 3) {
                //     // threefold repetition
                //     state.left_info = "Game drawn by threefold repetition";
                //     this.stop();
                // }
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
        state.game_btn_text = 'Start Game';

        state.w_conn_btn_state = 'button-to-connect';
        state.w_conn_btn_text  = 'Connect White Bot';
        state.b_conn_btn_state = 'button-to-connect';
        state.b_conn_btn_text  = 'Connect Black Bot';

        clearInterval(this.w_timer);
        clearInterval(this.b_timer);
        state.w_time_left = 0;
        state.b_time_left = 0;

        this.over = true;
        state.in_progress = false;
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
}

function set_8_4_board() {
    state.board_class = 'board-8-4';
}

function set_8_2_board() {
    state.board_class = 'board-8-2';
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
    <div class='button-enclose'><button :class='state.game_btn_state' @click="state.game.start_stop" v-html="state.game_btn_text"></button></div>
</div>
<div class='timer'>
    <div class='time-left'>White: {{state.w_time_left}}</div>
    <div class='time-left'>Black: {{state.b_time_left}}</div>
</div>

<Board ref='board' :type='state.board_class' :moves="state.game !== null ? state.game.move_list : []" id='myBoard'/>

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
</style>
