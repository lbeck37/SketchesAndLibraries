const char index_html[] PROGMEM = R"rawliteral(
<!-- BeckAsyncWebServerHTML.h, 8/10/19b -->
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.8.1/css/all.css" integrity="sha384-50oBUHEmvpQ+1lW4y57PTFmhCaXp0ML5d60M1M7uH2+nqUivzIebhndOJK28anvf" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>BIOTA</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
    <span class="dht-labels">Current</span>
    <span id="LastDegF">%TEMPERATURE%</span>
    <sup class="units">&deg;F</sup>
  </p>
  <p>
    <i class="fas fa-tachometer-alt" style="color: Tomato;"></i>
    <span class="dht-labels">Setpoint</span>
    <span id="SetPointDegF">%SET_POINT%</span>
    <sup class="units">&deg;F</sup>
  </p>
  <p>
    <i class="fas fa-stroopwafel fa-spin" style="color: Dodgerblue;"></i>
    <span class="dht-labels">Offpoint</span>
    <span id="TermoOffDegF">%THERMO_OFF%</span>
    <sup class="units">&deg;F</sup>
  </p>
</body>

<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LastDegF").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/LastDegF", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("SetPointDegF").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/SetPointDegF", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TermoOffDegF").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/TermoOffDegF", true);
  xhttp.send();
}, 1000 ) ;

</script>
</html>)rawliteral";
