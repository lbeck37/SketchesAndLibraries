const char index_html[] PROGMEM = R"rawliteral(
<!-- BeckAsyncWebServerHTML.h, 4/13/19a AngularJS Example-->
<!DOCTYPE HTML><html>
<!doctype html>
<html ng-app>
  <head>
    <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.7.8/angular.min.js"></script>
  </head>
  <body>
    <div>
      <label>Name:</label>
      <input type="text" ng-model="yourName" placeholder="Enter a name here">
      <hr>
      <h1>Hello {{yourName}}!</h1>
    </div>
  </body>
</html>)rawliteral";

