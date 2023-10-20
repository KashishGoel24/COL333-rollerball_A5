<script setup>
import ChessBoard from 'chessboardjs-vue3';
import Board from './components/Board.vue'
import {onMounted, reactive, watch, computed} from 'vue';

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

    if      (command === 'uciok')        on_uciok(side, tokens);
    else if (command === 'ucinewgameok') on_ucinewgameok(side, tokens);
    else if (command === 'bestmove')     on_bestmove(side, tokens);
    else {
        stop_game(`Unknown command ${command} received from ${side}`);
    }
}

function connect_socket(side) {

    state.sockets[side].state  = 'connecting';
    state.sockets[side].socket = new WebSocket(`ws://${state.sockets[side].address}`);

    state.sockets[side].socket.onopen = (event) => {
        state.sockets[side].socket.send("uci");
    }

    state.sockets[side].socket.onerror = (err) => {
        state.right_info = `Could not connect to ${side} bot`;
        state.sockets[side].state = 'disconnected';
    }
    
    state.sockets[side].socket.onmessage = (msg) => {
        process_message(side, msg.data);
    }
}

function disconnect_socket(side) {
    state.sockets[side].socket.close();
    state.sockets[side].state = 'disconnected';
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

//--- Dependencies ------------------------------------------------------------

function disable_on_disconnect(s) {
    if (s === 'disconnected' || s === 'connecting') {
        state.game.state = 'idle';
    }
}

watch(() => state.sockets.white.state, disable_on_disconnect);
watch(() => state.sockets.black.state, disable_on_disconnect);

//-- UI Stuff ----------------------------------------------------------------- 


function socket_btn_disabled_closure(side) {
    return () => {
        if ((state.game.state === 'idle'  || 
             state.game.state === 'ready' || 
             state.game.state === 'final') && 
             state.sockets[side].state !== 'connecting') {
            return false;
        }
        return true;
    }
}

function socket_btn_class_closure(side) {

    return () => {
        if (state.sockets[side].state === 'connected') return 'button-connected';
        else if (state.sockets[side].state === 'connecting') return 'button-connecting';
        else if (state.sockets[side].state === 'disconnected') return 'button-to-connect';
        else {
            console.log('ERROR: Invalid socket state');
            return 'button-to-connect';
        }
    }
}

function socket_btn_text_closure(side) {

    return () => {
        if (state.sockets[side].state === 'connected') return `Disconnect ${side} bot`;
        else if (state.sockets[side].state === 'connecting') return `Connecting ${side} bot`;
        else if (state.sockets[side].state === 'disconnected') return `Connect ${side} bot`;
        else {
            return 'ERROR: Invalid socket state';
        }
    }
}

const white_socket_btn_disabled = computed(socket_btn_disabled_closure('white'));
const black_socket_btn_disabled = computed(socket_btn_disabled_closure('black'));

const white_socket_btn_class = computed(socket_btn_class_closure('white'));
const black_socket_btn_class = computed(socket_btn_class_closure('black'));

const white_socket_btn_text = computed(socket_btn_text_closure('white'));
const black_socket_btn_text = computed(socket_btn_text_closure('black'));

function socket_btn_click(side) {
    if (state.sockets[side].state === 'connected') disconnect_socket(side);
    else connect_socket(side);
}

</script>

<template>

<form>
    <div class='field-pad'>
        <label for="white_address">White Address:</label>
        <input id="white_address" v-model="state.sockets.white.address" placeholder="white address">
    </div>
    <div class='field-pad'>
        <label for="black_address">Black Address:</label>
        <input id="black_address" v-model="state.sockets.black.address" placeholder="black address">
    </div>
    <div class='field-pad'>
        <label for="time_limit">Time Limit:</label>
        <input type="number" id="time_limit" v-model.number="state.timer.time_limit" placeholder="time limit">
    </div>
</form>
<div class='button-bar'>
    <div class='button-enclose'>
        <button :disabled='white_socket_btn_disabled' 
                :class='white_socket_btn_class' 
                @click="socket_btn_click('white')" 
                v-html="white_socket_btn_text"></button>
    </div>
    <div class='button-enclose'>
        <button :disabled='black_socket_btn_disabled' 
                :class='black_socket_btn_class' 
                @click="socket_btn_click('black')" 
                v-html="black_socket_btn_text"></button>
    </div>
    <div class='button-enclose'>
        <button :class='state.game_btn_state' 
                @click="state.game.start_stop" 
                v-html="state.game_btn_text"></button>
    </div>
</div>
<div class='timer'>
    <div class='time-left'>White: {{Math.ceil(state.timer.white.time_ms / 1000)}}</div>
    <div class='time-left'>Black: {{Math.ceil(state.timer.black.time_ms / 1000)}}</div>
</div>

<Board ref='board' :type='state.game.type' :moves="state.game.move_list" id='myBoard'/>

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
