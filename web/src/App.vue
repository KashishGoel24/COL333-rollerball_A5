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

    ws_white: null,
    ws_black: null
})

function connect_white() {
    console.log("Connecting to white\n");

    state.ws_white = new WebSocket("ws://localhost:8181");
    state.ws_white.onopen = (event) => {
        console.log("Connected\n");
        // reset the button state
    }
}

function connect_black() {
    console.log("Connecting to black\n");

    state.ws_black = new WebSocket("ws://localhost:8182");
    // set button state to connecting
    state.ws_black.onopen = (event) => {
        console.log("Connected\n");
        // reset the button state
    }
}

</script>

<template>
<form>
    <label for="white_port">White port</label>
    <input type="number" id="white_port" v-model.number="state.white_port" placeholder="white port">
    <label for="black_port">Black port</label>
    <input type="number" id="black_port" v-model.number="state.black_port" placeholder="black port">
    <label for="thinking_time">Thinking time</label>
    <input type="number" id="thinking_time" v-model.number="state.thinking_time" placeholder="thinking time">
</form>
    <button class='button-hit' @click="connect_white">Connect White</button>
    <button class='button-hit' @click="connect_black">Connect Black</button>
    <button class='button-hit' @click="start">Start</button>

<div id="myBoard" style="width: 400px"></div>
</template>

<style scoped>
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
