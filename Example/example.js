setTimeout(()=>{alert("HELOOOO!!!")},2000)
const serverURL = window.location.href
const textinput = document.getElementById("textinput")
const sendbutton = document.getElementById("sendbutton")
const xhr = new XMLHttpRequest();

sendbutton.addEventListener("click",()=>{
  if(textinput.value!=""){
    xhr.open("POST", "http://localhost:5500");
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8")
    xhr.send(`{message:${textinput.value}}`);
    textinput.value = ""
  }
})

