(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([["chunk-44dd1190"],{"736f":function(e,t,o){"use strict";o("88c8")},"88c8":function(e,t,o){},"8cf5":function(e,t,o){"use strict";o.r(t);var n=o("fc6a");const l={class:"setting"};function c(e,t,o,c,a,r){const i=Object(n["resolveComponent"])("el-button"),s=Object(n["resolveComponent"])("el-header"),d=Object(n["resolveComponent"])("CreateConf"),b=Object(n["resolveComponent"])("el-tab-pane"),u=Object(n["resolveComponent"])("RemoveConfig"),p=Object(n["resolveComponent"])("ListConfig"),m=Object(n["resolveComponent"])("el-tabs"),j=Object(n["resolveComponent"])("el-main"),O=Object(n["resolveComponent"])("el-container");return Object(n["openBlock"])(),Object(n["createElementBlock"])("div",l,[Object(n["createVNode"])(O,null,{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(s,null,{default:Object(n["withCtx"])(()=>[Object(n["createElementVNode"])("span",null,Object(n["toDisplayString"])("当前地址: "+e.$store.state.backendAddr),1),Object(n["createElementVNode"])("p",null,[Object(n["createVNode"])(i,{type:"success",size:"small",onClick:r.callSetting},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("设置")]),_:1},8,["onClick"])])]),_:1}),Object(n["createVNode"])(j,null,{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(m,{type:"border-card",class:"cardsetting"},{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(b,{label:"创建配置"},{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(d,{serverEndpoint:e.$store.state.backendAddr},null,8,["serverEndpoint"])]),_:1}),Object(n["createVNode"])(b,{label:"删除配置"},{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(u,{serverEndpoint:e.$store.state.backendAddr},null,8,["serverEndpoint"])]),_:1}),Object(n["createVNode"])(b,{label:"查询配置"},{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(p,{serverEndpoint:e.$store.state.backendAddr},null,8,["serverEndpoint"])]),_:1})]),_:1})]),_:1})]),_:1})])}var a=o("8be6"),r=o.n(a);function i(e,t,o,l,c,a){const r=Object(n["resolveComponent"])("el-tag"),i=Object(n["resolveComponent"])("el-input"),s=Object(n["resolveComponent"])("el-autocomplete"),d=Object(n["resolveComponent"])("el-divider"),b=Object(n["resolveComponent"])("el-button");return Object(n["openBlock"])(),Object(n["createElementBlock"])("div",null,[Object(n["createElementVNode"])("p",null,[Object(n["createVNode"])(r,{size:"medium"},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("配置名")]),_:1}),Object(n["createVNode"])(i,{type:"text",modelValue:c.confName,"onUpdate:modelValue":t[0]||(t[0]=e=>c.confName=e),modelModifiers:{lazy:!0,trim:!0},size:"small",style:{width:"40%"},clearable:""},null,8,["modelValue"])]),Object(n["createElementVNode"])("p",null,[Object(n["createVNode"])(r,{size:"medium"},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("版本")]),_:1}),Object(n["createVNode"])(i,{type:"text",modelValue:c.confVersion,"onUpdate:modelValue":t[1]||(t[1]=e=>c.confVersion=e),modelModifiers:{lazy:!0,trim:!0},size:"small",style:{width:"40%"},clearable:""},null,8,["modelValue"])]),Object(n["createElementVNode"])("p",null,[Object(n["createVNode"])(r,{size:"medium"},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("类型")]),_:1}),Object(n["createVNode"])(s,{class:"inline-input",modelValue:c.confType,"onUpdate:modelValue":t[2]||(t[2]=e=>c.confType=e),modelModifiers:{lazy:!0,trim:!0},"fetch-suggestions":a.querySearch,placeholder:"json",style:{width:"20%"},clearable:""},null,8,["modelValue","fetch-suggestions"])]),Object(n["createVNode"])(d),Object(n["createElementVNode"])("p",null,[Object(n["createVNode"])(r,null,{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("配置内容")]),_:1}),Object(n["createVNode"])(b,{type:"success",size:"small",onClick:t[3]||(t[3]=e=>a.configCreate(o.serverEndpoint))},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("创建")]),_:1})]),Object(n["createElementVNode"])("p",null," 返回值 ： "+Object(n["toDisplayString"])(c.server.code)+" 返回信息： "+Object(n["toDisplayString"])(c.server.msg),1),Object(n["createVNode"])(i,{type:"textarea",modelValue:c.confContent,"onUpdate:modelValue":t[4]||(t[4]=e=>c.confContent=e),modelModifiers:{lazy:!0,trim:!0},style:{width:"80%",overflow:"scroll"}},null,8,["modelValue"])])}var s={data(){return{confName:"conf_name",confVersion:"0.1.2",confContent:"content",confType:"json",typeList:[{value:"json",label:"json"},{value:"gflags",label:"gflags"},{value:"text",label:"text"},{value:"toml",label:"toml"},{value:"xml",label:"xml"},{value:"yaml",label:"yaml"},{value:"ini",label:"ini"}],server:{code:0,msg:"null"}}},props:{serverEndpoint:{type:String,required:!0}},methods:{configCreate(e){r.a.post("http://"+e+"/ea/config/create",{name:this.confName,version:this.confVersion,content:this.confContent,type:this.confType}).then(e=>{this.server.code=e.data.code,this.server.msg=e.data.message})},querySearch(e,t){var o=this.typeList,n=e?o.filter(this.createFilter(e)):o;t(n)},createFilter(e){return t=>0===t.value.toLowerCase().indexOf(e.toLowerCase())}}},d=o("9de4"),b=o.n(d);const u=b()(s,[["render",i]]);var p=u;const m={slot:"header",class:"clearfix"};function j(e,t,o,l,c,a){const r=Object(n["resolveComponent"])("el-tag"),i=Object(n["resolveComponent"])("el-input"),s=Object(n["resolveComponent"])("el-checkbox"),d=Object(n["resolveComponent"])("el-button");return Object(n["openBlock"])(),Object(n["createElementBlock"])("div",m,[Object(n["createVNode"])(r,null,{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("配置名")]),_:1}),Object(n["createVNode"])(i,{type:"text",modelValue:c.confName,"onUpdate:modelValue":t[0]||(t[0]=e=>c.confName=e),modelModifiers:{lazy:!0,trim:!0}},null,8,["modelValue"]),Object(n["createVNode"])(r,null,{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("版本")]),_:1}),Object(n["createVNode"])(s,{modelValue:c.allVersion,"onUpdate:modelValue":t[1]||(t[1]=e=>c.allVersion=e),onClick:a.OnChangeAllVersion,onChange:a.OnChangeAllVersion},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("全部版本")]),_:1},8,["modelValue","onClick","onChange"]),Object(n["createVNode"])(i,{type:"text",modelValue:c.confVersion,"onUpdate:modelValue":t[2]||(t[2]=e=>c.confVersion=e),modelModifiers:{lazy:!0,trim:!0},disabled:c.disableEditVersion},null,8,["modelValue","disabled"]),Object(n["createVNode"])(d,{type:"success",onClick:t[3]||(t[3]=e=>a.configRemove(o.serverEndpoint))},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("删除")]),_:1}),Object(n["createElementVNode"])("p",null,"返回值 ： "+Object(n["toDisplayString"])(c.server.code),1),Object(n["createElementVNode"])("p",null,"返回信息： "+Object(n["toDisplayString"])(c.server.msg),1)])}var O={data(){return{confName:"conf_name",confVersion:"0.1.2",allVersion:!1,disableEditVersion:!1,server:{code:0,msg:"null"}}},props:{serverEndpoint:{type:String,required:!0}},methods:{configRemove(e){r.a.post("http://"+e+"/ea/config/remove",{name:this.confName,version:this.confVersion}).then(e=>{this.server.code=e.data.code,this.server.msg=e.data.message})},OnChangeAllVersion(){this.allVersion?(this.confVersion="",this.disableEditVersion=!0):(this.confVersion="0.1.2",this.disableEditVersion=!1)}}};const f=b()(O,[["render",j]]);var h=f;const V={slot:"header",class:"clearfix"},g={class:"block"};function C(e,t,o,l,c,a){const r=Object(n["resolveComponent"])("el-tag"),i=Object(n["resolveComponent"])("el-input"),s=Object(n["resolveComponent"])("el-button"),d=Object(n["resolveComponent"])("el-table-column"),b=Object(n["resolveComponent"])("el-table"),u=Object(n["resolveComponent"])("el-pagination"),p=Object(n["resolveComponent"])("el-col"),m=Object(n["resolveComponent"])("el-row"),j=Object(n["resolveDirective"])("loading");return Object(n["openBlock"])(),Object(n["createBlock"])(m,null,{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(p,{span:24},{default:Object(n["withCtx"])(()=>[Object(n["createElementVNode"])("div",V,[Object(n["createVNode"])(r,null,{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("配置名")]),_:1}),Object(n["createTextVNode"])(),Object(n["createVNode"])(i,{type:"text",modelValue:c.queryConfig,"onUpdate:modelValue":t[0]||(t[0]=e=>c.queryConfig=e),modelModifiers:{lazy:!0,trim:!0},placeholder:"config name",clearable:""},null,8,["modelValue"]),Object(n["createVNode"])(r,null,{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("版本")]),_:1}),Object(n["createTextVNode"])(),Object(n["createVNode"])(i,{type:"text",modelValue:c.queryVersion,"onUpdate:modelValue":t[1]||(t[1]=e=>c.queryVersion=e),modelModifiers:{lazy:!0,trim:!0},placeholder:"0.0.1",clearable:""},null,8,["modelValue"]),Object(n["createVNode"])(s,{type:"success",onClick:t[2]||(t[2]=e=>a.configQueryProxy(o.serverEndpoint))},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("查询")]),_:1}),Object(n["createElementVNode"])("p",null,"返回值 ： "+Object(n["toDisplayString"])(c.server.code),1),Object(n["createElementVNode"])("p",null,"返回信息： "+Object(n["toDisplayString"])(c.server.msg),1),Object(n["createElementVNode"])("p",null,"配置列表： "+Object(n["toDisplayString"])(c.total),1)]),Object(n["withDirectives"])((Object(n["openBlock"])(),Object(n["createBlock"])(b,{stripe:"stripe",border:"border",onCellDblclick:a.copyText,data:c.configArray.slice((c.currentPage-1)*c.pagesize,c.currentPage*c.pagesize),style:{width:"100%"}},{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(d,{prop:"id",label:"配置ID",width:"90"}),Object(n["createVNode"])(d,{prop:"name",label:"配置名称",width:"180"}),Object(n["createVNode"])(d,{prop:"version",label:"版本",width:"90"}),Object(n["createVNode"])(d,{prop:"type",label:"类型",width:"90"}),Object(n["createVNode"])(d,{label:"操作",width:"90"},{default:Object(n["withCtx"])(({row:e,$index:t})=>[Object(n["createVNode"])(s,{type:"success",size:"mini",onClick:o=>a.handleQuery(t,e)},{default:Object(n["withCtx"])(()=>[Object(n["createTextVNode"])("查看")]),_:2},1032,["onClick"])]),_:1})]),_:1},8,["onCellDblclick","data"])),[[j,c.loading]]),Object(n["createElementVNode"])("div",g,[Object(n["createVNode"])(u,{layout:"prev, pager, next",onCurrentChange:a.current_change,"page-size":c.pagesize,total:c.total},null,8,["onCurrentChange","page-size","total"])])]),_:1}),Object(n["createVNode"])(p,{span:12},{default:Object(n["withCtx"])(()=>[Object(n["createVNode"])(i,{type:"textarea",modelValue:c.queryConfigContent,"onUpdate:modelValue":t[3]||(t[3]=e=>c.queryConfigContent=e),modelModifiers:{lazy:!0,trim:!0},placeholder:"",clearable:""},null,8,["modelValue"])]),_:1})]),_:1})}var v={data(){return{queryVersion:"",queryConfig:"",queryConfigContent:"",currentPage:1,loading:!1,pagesize:5,total:0,configArray:[],server:{code:0,msg:"null"}}},props:{serverEndpoint:{type:String,required:!0}},methods:{configQueryProxy(e){this.loading=!0,""!==this.queryVersion&&""!==this.queryConfig?this.configQuery(e):""===this.queryVersion&&""!==this.queryConfig?this.configVersionList(e):this.configList(e),this.loading=!1},configList(e){r.a.post("http://"+e+"/ea/config/list",{}).then(e=>{this.server.code=e.data.code,this.server.msg=e.data.message,"configs"in e.data?(this.configArray=e.data.configs,this.total=this.configArray.length):(this.configArray=[],this.total=0)})},configVersionList(e){r.a.post("http://"+e+"/ea/config/version",{name:this.queryConfig}).then(e=>{this.server.code=e.data.code,this.server.msg=e.data.message,"configs"in e.data?(this.configArray=e.data.configs,this.total=this.configArray.length):(this.configArray=[],this.total=0)})},configQuery(e){r.a.post("http://"+e+"/ea/config/query",{name:this.queryConfig,version:this.queryVersion}).then(e=>{this.server.code=e.data.code,this.server.msg=e.data.message,"configs"in e.data?(this.configArray=e.data.configs,this.total=this.configArray.length):(this.configArray=[],this.total=0)})},handleQuery(e,t){this.queryConfigContent=t.content},copyText(e,t,o,n){let l=function(e){e.clipboardData.setData("text/plain",n.target.innerText),e.preventDefault()};document.addEventListener("copy",l),document.execCommand("copy"),this.$notify({message:"恭喜你，复制成功！"+n.target.innerText,type:"success"})},current_change(e){this.currentPage=e}}};const y=b()(v,[["render",C]]);var N=y,x={components:{CreateConf:p,RemoveConfig:h,ListConfig:N},data(){return{serverEndpoint:"localhost:8018",labelType:!0,testResult:"失败",buttonType:"danger"}},methods:{callSetting(){this.$emit("api",{event:"openApp",app:"system_setting"})}},created(){}};o("736f");const w=b()(x,[["render",c],["__scopeId","data-v-80c70c6a"]]);t["default"]=w}}]);
//# sourceMappingURL=chunk-44dd1190.0df0838b.js.map