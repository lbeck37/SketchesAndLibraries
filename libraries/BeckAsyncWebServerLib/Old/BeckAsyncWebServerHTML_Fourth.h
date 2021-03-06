const char index_html[] PROGMEM = R"rawliteral(
<!-- BeckAsyncWebServerHTML.h, 4/14/19c AngularJS Example -->
<!-- File: chapter6/public/http-post-example.html -->
<html ng-app="notesApp">

<head>
  <title>HTTP Post Example</title>
  <style>
    .item {
      padding: 10px;
    }
  </style>
</head>

<body ng-controller="MainCtrl as mainCtrl">
  <h1>Hello Servers!</h1>
  <div ng-repeat="todo in mainCtrl.items"
       class="item">
    <div><span ng-bind="todo.label"></span></div>
    <div>- by <span ng-bind="todo.author"></span></div>
  </div>

  <div>
    <form name="addForm"
          ng-submit="mainCtrl.add()">
      <input type="text"
             placeholder="Label"
             ng-model="mainCtrl.newTodo.label"
             required>
      <input type="text"
             placeholder="Author"
             ng-model="mainCtrl.newTodo.author"
             required>
      <input type="submit"
             value="Add"
             ng-disabled="addForm.$invalid">
    </form>
  </div>

<script
  src="https://ajax.googleapis.com/ajax/libs/angularjs/1.3.11/angular.js">
</script>
<script>
  angular.module('notesApp', [])
    .controller('MainCtrl', ['$http', function($http) {
      var self = this;
      self.items = [];
      self.newTodo = {};
      var fetchTodos = function() {
        return $http.get('/api/note').then(
            function(response) {
          self.items = response.data;
        }, function(errResponse) {
          console.error('Error while fetching notes');
        });
      };

      fetchTodos();

      self.add = function() {
        $http.post('/api/note', self.newTodo)
            .then(fetchTodos)
            .then(function(response) {
              self.newTodo = {};
            });
      };

    }]);
</script>
</body>
</html>
)rawliteral";

