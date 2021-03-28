// BeckThermoFirebaseHTML.h
#pragma once
const char* acThermoFirebaseHTML= R"(
<!DOCTYPE HTML>
<html ng-app="ThermoApp">
<head>
  <meta charset="utf-8">
  <title>My Site</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <!-- AngularJS.js -->
      <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.7.8/angular.min.js"></script>
    <!-- Firebase.js -->
      <script src="https://www.gstatic.com/firebasejs/3.1.0/firebase.js"></script>
    <!-- AngularFire.js -->
      <script src="https://cdn.firebase.com/libs/angularfire/2.3.0/angularfire.min.js"></script>
    <!-- Bootstrap.css -->
      <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">
    <!-- FontAwesome.css -->
      <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.8.1/css/all.css" integrity="sha384-50oBUHEmvpQ+1lW4y57PTFmhCaXp0ML5d60M1M7uH2+nqUivzIebhndOJK28anvf" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: left;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      <!-- vertical-align:middle;-->
      padding-bottom: 15px;
    }
    #number {
      width: 3em;
    }
  </style>
</head>
<body ng-controller="MainCtrl as ctrl">
  <div class="container">
<!-- ****** Date Code ****** -->
   <h2>Thermostat 8/21/19e</h2>
   <form ng-submit="ctrl.submit() ">
<!-- LastDegF -->
     <p>
        <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
        <span class="dht-labels">Current</span>
        <span id="LastDegF">{{ctrl.ThermoObj.DegF}}</span>
        <sup class="units">&deg;F</sup>
      </p>
<!-- SetPointDegF -->
      <p>
        <i class="fas fa-tachometer-alt" style="color: Tomato;"></i>
        <span class="dht-labels">Setpoint</span>
        <span id="SetPointDegF">{{ctrl.ThermoObj.Setpoint}}</span>
        <sup class="units">&deg;F</sup>
      </p>
 <!-- DeltaOff -->
     <p>
        <i class="fas fa-stroopwafel fa-spin" style="color: Dodgerblue;"></i>
        <span class="dht-labels">DeltaOff</span>
        <span id="DeltaOff">{{ctrl.ThermoObj.DeltaOff}}</span>
       <sup class="units">&deg;F</sup>
      </p>
<!-- Offpoint, calculated from database values -->
      <p>
        <i class="fas fa-stroopwafel fa-spin" style="color: Dodgerblue;"></i>
        <span class="dht-labels">Offpoint</span>
        <span id="TermoOffDegF">{{ctrl.dThermoOffDegF}}</span>
        <sup class="units">&deg;F</sup>
      </p>
<!-- New Setpoint, calculated from database values -->
      <p>
  <!-- <label>New Setpoint:</label> -->
  <!-- <input type="number" step="0.1" ng-model="ctrl.ThermoObj.Setpoint"> -->
       <span class="dht-labels">New Setpoint</span>
       <input type="number" id="number" step="0.1" ng-model="ctrl.ThermoObj.Setpoint">
      </p>
<!-- New DeltaOff, calculated from database values -->
      <p>
  <!-- <label>New DeltaOff:</label> -->
  <!-- <input type="number" step="0.01" ng-model="ctrl.ThermoObj.DeltaOff"> -->
       <span class="dht-labels">New DeltaOff</span>
       <input type="number" id="number" step="0.1" ng-model="ctrl.ThermoObj.DeltaOff">
    </p>
      <p>
        <input type="submit" value="Update">
      </p>
    </form>
  </div> <!-- class="container" -->

  <!-- jQuery first, then Popper.js, then Bootstrap JS -->
  <!-- jQuery.js -->
  <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
  <!-- Popper.js -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
  <!-- Bootstrap.js -->
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
  
<!-- JavaScript code -->
  <script type="text/javascript">
    var ThermoModule= angular.module('ThermoApp', ['firebase']);
    var firebaseConfig = {
        apiKey: "AIzaSyBVOUnaaCjuoFWLsiyVi7ZSW_PsiHLam1A",
        authDomain: "fir-mariestep1.firebaseapp.com",
        databaseURL: "https://fir-mariestep1.firebaseio.com",
        projectId: "fir-mariestep1",
        storageBucket: "fir-mariestep1.appspot.com",
        messagingSenderId: "754022099819",
        appId: "1:754022099819:web:50b456b2ed48aa27"
      };
    //Initialize Firebase app
    firebase.initializeApp(firebaseConfig);

    ThermoModule.controller('MainCtrl', ['$firebaseObject',
      function($firebaseObject){
        var self= this;
        //Create FirebaseObject
          var ThermoRef = firebase.database().ref('Thermo');
          self.ThermoObj= $firebaseObject(ThermoRef);

        //Wait for data from Firebase
        self.ThermoObj.$loaded().then(function() {
          self.dThermoOffDegF= self.ThermoObj.Setpoint + self.ThermoObj.DeltaOff;
          console.log("dThermoOffDegF= " + self.dThermoOffDegF);
        });
        self.submit= function(){
          console.log("submit(): self.ThermoObj.Setpoint= " + self.ThermoObj.Setpoint);
          self.dThermoOffDegF= self.ThermoObj.Setpoint + self.ThermoObj.DeltaOff;
          self.ThermoObj.$save().then(function(){
            console.log('submit(): ThermoObj saved back to Firebase');
            }).catch(function(error){
              console.log('submit(): ThermoObj saved back to Firebase');
            });
        };  //submit
        return;
      } //function()
    ]);
  </script>
</body>
</html>
)";
//Last line.
