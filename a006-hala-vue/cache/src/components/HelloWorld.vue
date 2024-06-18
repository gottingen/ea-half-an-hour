<template>
  <div class="hello">
    <h1>{{ name }}</h1>
  </div>
  <div class="query_table">
    <p>
      请输入缓存key：
      <input type="text" v-model.lazy.trim="cache_key">
    </p>
    <p>
      请输入缓存值：
      <textarea v-model.lazy="cache_value"></textarea>
    </p>
    <p>
      <input type="button" value="查询缓存" @click="get_cache_value">
      <input type="button" value="设置缓存" @click="set_cache_value">
    </p>

    <table align="center" border="1">
      <tr>
        <td >缓存key</td>
        <td>返回码</td>
        <td>返回信息</td>
        <td>缓存值</td>
      </tr>
      <tr>
        <td>{{ cache_key }}</td>
        <td>{{ code }}</td>
        <td>{{ msg }}</td>
        <td>{{ cache_value }}</td>
      </tr>
    </table>
  </div>
</template>

<script>

import querystring from "querystring";

export default {
  data () {
    return {
      name: 'HelloWorld Vita',
      cache_key: 'cachekey',
      cache_value: 'cachevalue',
      server: 'localhost:8018',
      code: 0,
      msg: '',
    }
  },
  methods: {
    get_cache_value: function() {
      this.$axios.get('http://' + this.server + '/ea/cache?key=' + this.cache_key)
          .then(response => {
            this.code = response.data.code;
            this.msg = response.data.msg;
            this.cache_value = response.data.value;
          })
    },
    set_cache_value: function() {
      this.$axios.post('http://' + this.server + '/ea/cache?key=' + this.cache_key,this.cache_value)
          .then(response => {
            this.code = response.data.code;
            this.msg = response.data.msg;
          })
    }
  }
}
</script>
