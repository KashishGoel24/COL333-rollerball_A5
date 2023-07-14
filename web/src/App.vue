<script setup>
import ChessBoard from 'chessboardjs-vue3';
import {onMounted, reactive} from 'vue';

onMounted(() => {
    var board = ChessBoard('myBoard', {
        'position': '8/2rbp3/2rkp3/8/8/8/2PKR3/2PBR3',
        'draggable': true,
    })
});

const state = reactive({
    white_port: 8181,
    black_port: 8182,
    thinking_time: 2,

    w_conn_btn_state: 'button-to-connect',
    w_conn_btn_text: 'Connect White Bot',
    b_conn_btn_state: 'button-to-connect',
    b_conn_btn_text: 'Connect Black Bot',
    game_btn_state: 'button-to-start',
    game_btn_text: 'Start Game',

    left_info: 'rollerball v.1.0',
    right_info: 'Copyright COL333 TAs',

    ws_white: null,
    ws_black: null
})

function connect_white() {
    console.log("Connecting to white\n");

    state.w_conn_btn_state = 'button-connecting';
    state.w_conn_btn_text = 'Connecting White Bot';
    state.ws_white = new WebSocket(`ws://localhost:${state.white_port}`);

    state.ws_white.onopen = (event) => {
        state.ws_white.send("uci");
        state.w_conn_btn_state = 'button-connected';
        state.w_conn_btn_text = 'Connected to White Bot';
        console.log("Connected\n");
    }

    state.ws_white.onerror = (err) => {
        state.left_info = 'Could not connect to white bot';
        state.w_conn_btn_state = 'button-to-connect';
        state.w_conn_btn_text = 'Connect White Bot';
    }

}

function connect_black() {
    console.log("Connecting to black\n");

    state.b_conn_btn_state = 'button-connecting';
    state.b_conn_btn_text = 'Connecting Black Bot';
    state.ws_black = new WebSocket(`ws://localhost:${state.black_port}`);

    state.ws_black.onopen = (event) => {
        state.ws_white.send("uci");
        state.b_conn_btn_state = 'button-connected';
        state.b_conn_btn_text = 'Connected to Black Bot';
        console.log("Connected\n");
    }

    state.ws_black.onerror = (err) => {
        state.left_info = 'Could not connect to black bot';
        state.b_conn_btn_state = 'button-to-connect';
        state.b_conn_btn_text = 'Connect Black Bot';
    }
}

function start() {

}

</script>

<template>

<form>
    <div class='field-pad'>
        <label for="white_port">White port:</label>
        <input type="number" id="white_port" v-model.number="state.white_port" placeholder="white port">
    </div>
    <div class='field-pad'>
        <label for="black_port">Black port:</label>
        <input type="number" id="black_port" v-model.number="state.black_port" placeholder="black port">
    </div>
    <div class='field-pad'>
        <label for="thinking_time">Thinking time:</label>
        <input type="number" id="thinking_time" v-model.number="state.thinking_time" placeholder="thinking time">
    </div>
</form>
<div class='button-bar'>
    <div class='button-enclose'><button :class='state.w_conn_btn_state' @click="connect_white" v-html="state.w_conn_btn_text"></button></div>
    <div class='button-enclose'><button :class='state.b_conn_btn_state' @click="connect_black" v-html="state.b_conn_btn_text"></button></div>
    <div class='button-enclose'><button :class='state.game_btn_state' @click="start" v-html="state.game_btn_text"></button></div>
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
  padding: 4px;
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
