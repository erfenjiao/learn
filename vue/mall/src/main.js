// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'
import router from './router'
//import { ElButton, ElContainer, ElAside, ElHeader, ElMain, ElFooter, ElMenu, ElSubmenu, ElMenuItemGroup, ElMenuItem, ElForm, ElFormItem, ElInput, ElPopover, ElTag, ElCard, ElTable, ElTableColumn, ElPagination, ElDialog, ElPopconfirm, ElUpload, ElLoading, ElSelect, ElOption, ElRadioGroup, ElRadio, ElCascader, ElCheckbox, ElInputNumber } from 'element-plus'

Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  components: { App },
  template: '<App/>'
})

// 使用 createApp 来创建 vue 实例
const app = createApp(App)
// 全局过滤器
app.config.globalProperties.$filters = {
  orderMap(status) {
    return orderStatus[status] || '未知状态'
  },
  prefix(url) {
    if (url && url.startsWith('http')) {
      return url
    } else {
      url = `http://backend-api-02.newbee.ltd${url}`
      return url
    }
  },
  resetImgUrl(imgObj, imgSrc, maxErrorNum) {  
    if (maxErrorNum > 0) { 
      imgObj.onerror = function() {  
        resetImgUrl(imgObj, imgSrc, maxErrorNum - 1) 
      }
      setTimeout(function() {  
        imgObj.src = imgSrc  
      }, 500)
    } else {  
      imgObj.onerror = null  
      imgObj.src = imgSrc
    }  
  }  
}

app.use(router)

// app.use(ElButton)
//     .use(ElContainer)
//     .use(ElAside)
//     .use(ElHeader)
//     .use(ElMain)
//     .use(ElFooter)
//     .use(ElMenu)
//     .use(ElSubmenu)
//     .use(ElMenuItemGroup)
//     .use(ElMenuItem)
//     .use(ElForm)
//     .use(ElFormItem)
//     .use(ElInput)
//     .use(ElPopover)
//     .use(ElTag)
//     .use(ElCard)
//     .use(ElTable)
//     .use(ElTableColumn)
//     .use(ElPagination)
//     .use(ElDialog)
//     .use(ElPopconfirm)
//     .use(ElUpload)
//     .use(ElLoading)
//     .use(ElSelect)
//     .use(ElOption)
//     .use(ElRadioGroup)
//     .use(ElRadio)
//     .use(ElCascader)
//     .use(ElCheckbox)
//     .use(ElInputNumber)

app.mount('#app')