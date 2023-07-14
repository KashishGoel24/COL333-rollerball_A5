<script setup>
import ChessBoard from 'chessboardjs-vue3';
import {onMounted} from 'vue';

onMounted(() => {
    var board = ChessBoard('myBoard', {
        'position': '8/2rbp3/2rkp3/8/8/8/2PKR3/2PBR3',
        'draggable': true,
    })
});

const state = reactive({
    white_port: 8181,
    black_port: 8182
})

</script>

<template>
<form>
<input type="number" v-model.number="state.white_port" placeholder="white port">
<input type="number" v-model.number="state.black_port" placeholder="black port">
</form>
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
