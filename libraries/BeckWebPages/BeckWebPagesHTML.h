// BeckWebPagesHTML.h, 5/7/2019a
#pragma once

const char* acThermoWebPageHTML= R"(
<!-- BeckWebPagesHTML.h, 5/6/19c -->
<!DOCTYPE HTML>
<html ng-app="ThermoApp">
<script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.6.9/angular.min.js"></script>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">

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
<body ng-controller="MainCtrl as ctrl">
  <div>
   <h2>Thermostat 5/6/19c</h2>
   <form ng-submit="ctrl.submit() ">
     <p>
        <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
        <span class="dht-labels">Current</span>
        <span id="LastDegF">{{ctrl.oThermo.dLastDegF}}</span>
        <sup class="units">&deg;F</sup>
      </p>
      <p>
        <i class="fas fa-tachometer-alt" style="color: Tomato;"></i>
        <span class="dht-labels">Setpoint</span>
        <span id="SetPointDegF">{{ctrl.oThermo.dSetpointF}}</span>
        <sup class="units">&deg;F</sup>
      </p>
      <p>
        <i class="fas fa-stroopwafel fa-spin" style="color: Dodgerblue;"></i>
        <span class="dht-labels">Max Heat Range</span>
        <span id="TermoOffDegF">{{ctrl.oThermo.dMaxHeatRangeF}}</span>
        <sup class="units">&deg;F</sup>
      </p>
      <p>
        <i class="fas fa-stroopwafel fa-spin" style="color: Dodgerblue;"></i>
        <span class="dht-labels">Offpoint</span>
        <span id="TermoOffDegF">{{ctrl.dThermoOffDegF}}</span>
        <sup class="units">&deg;F</sup>
      </p>
      <p>
      <input type="submit" value="New Values">
      </p>
    </form>
  </div>

  <script type="text/javascript">
    angular.module('ThermoApp', []).
      controller('MainCtrl', ['$http',
        function($http){
          var self= this;

          self.oThermo= {};
          self.oThermo.dLastDegF;
          self.oThermo.dSetpointF;
          self.oThermo.dMaxHeatRangeF;

          self.dThermoOffDegF;

//***Function declarations begin***
          self.DoThermoGet= function(){
            console.log('DoThermoGet(): Begin');
            ReturnVal= $http.get('/ThermoGet').
              then(function(response){
                self.oThermo= response.data;
                self.dThermoOffDegF= self.oThermo.dSetpointF + self.oThermo.dMaxHeatRangeF;
                console.log('DoThermoGet(): self.oThermo= ', self.oThermo);
              },
              function(errResponse){
                console.error('Error doing $http.get(/LastDegF)');
              });
            return ReturnVal;
          }; //DoThermoGet


          self.DoThermoPost= function(){
            console.log('DoThermoPost(): Begin');
            $http.post('/ThermoPost', self.oThermo).then(self.DoThermoGet)
          };  //DoThermoPost


          self.submit= function(){
            console.log('submit(): Begin');
            self.DoThermoPost();
            self.DoThermoGet();
          };  //submit
//***Function declarations end***

          self.DoThermoGet();
          return;
        } //function($http)
    ]);
</script>
    <!-- Bootstrap required JavaScript -->
    <!-- jQuery first, then Popper.js, then Bootstrap JS -->
    <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>

    <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
</body>
</html>
)";


const char* acTestPageHTML= R"(
<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="">
    <meta name="author" content="Mark Otto, Jacob Thornton, and Bootstrap contributors">
    <meta name="generator" content="Jekyll v3.8.5">
    <title>Starter Template � Bootstrap</title>

    <link rel="canonical" href="https://getbootstrap.com/docs/4.3/examples/starter-template/">

    <!-- Bootstrap core CSS -->
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css"
          integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">

    <style>
      .bd-placeholder-img {
        font-size: 1.125rem;
        text-anchor: middle;
        -webkit-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
      }

      @media (min-width: 768px) {
        .bd-placeholder-img-lg {
          font-size: 3.5rem;
        }
      }
    </style>

    <!-- Custom styles for this template -->
    <style>
      body {
        padding-top: 5rem;
      }
      .starter-template {
        padding: 3rem 1.5rem;
        text-align: center;
      }
    </style>

</head>
  <body>
    <nav class="navbar navbar-expand-md navbar-dark bg-dark fixed-top">
  <a class="navbar-brand" href="#">Navbar</a>

  <button class="navbar-toggler" type="button" data-toggle="collapse"
          data-target="#navbarsExampleDefault" aria-controls="navbarsExampleDefault"
          aria-expanded="false" aria-label="Toggle navigation">

    <span class="navbar-toggler-icon"></span>
  </button>

  <div class="collapse navbar-collapse" id="navbarsExampleDefault">
    <ul class="navbar-nav mr-auto">
      <li class="nav-item active">
        <a class="nav-link" href="#">Home <span class="sr-only">(current)</span></a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="#">Link</a>
      </li>
      <li class="nav-item">
        <a class="nav-link disabled" href="#" tabindex="-1" aria-disabled="true">Disabled</a>
      </li>
      <li class="nav-item dropdown">
        <a class="nav-link dropdown-toggle" href="#" id="dropdown01" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Dropdown</a>
        <div class="dropdown-menu" aria-labelledby="dropdown01">
          <a class="dropdown-item" href="#">Action</a>
          <a class="dropdown-item" href="#">Another action</a>
          <a class="dropdown-item" href="#">Something else here</a>
        </div>
      </li>
    </ul>
    <form class="form-inline my-2 my-lg-0">
      <input class="form-control mr-sm-2" type="text" placeholder="Search" aria-label="Search">
      <button class="btn btn-secondary my-2 my-sm-0" type="submit">Search</button>
    </form>
  </div>
</nav>

<main role="main" class="container">

  <div class="starter-template">
    <h1>Bootstrap starter template</h1>
    <p class="lead">Use this document as a way to quickly start any new project.<br> All you get is this text and a mostly barebones HTML document.</p>
  </div>

</main><!-- /.container -->
  <!-- Bootstrap required JavaScript -->
  <!-- jQuery first, then Popper.js, then Bootstrap JS -->

  <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>

  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>

  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
</body>
</html>
)";

//Last line.
