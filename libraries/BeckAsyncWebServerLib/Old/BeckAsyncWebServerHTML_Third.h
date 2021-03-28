const char index_html[] PROGMEM = R"rawliteral(
<!-- BeckAsyncWebServerHTML.h, 4/14/19b AngularJS Example-->
<!-- File: chapter5/simple-angularjs-service/index.html -->
<html ng-app="notesApp">
<body ng-controller="MainCtrl as mainCtrl">
  <h1>Hello Controllers!</h1>
  <button ng-click="mainCtrl.open('first')">
    Open First
  </button>
  <button ng-click="mainCtrl.open('second')">
    Open Second
  </button>
  <div ng-switch on="mainCtrl.tab">
    <div ng-switch-when="first">
      <div ng-controller="SubCtrl as ctrl">
        <h3>First tab</h3>
        <ul>
          <li ng-repeat="item in ctrl.list()">
            <span ng-bind="item.label"></span>
          </li>
        </ul>

        <button ng-click="ctrl.add()">
          Add More Items
        </button>
      </div>

    </div>
    <div ng-switch-when="second">
      <div ng-controller="SubCtrl as ctrl">
        <h3>Second tab</h3>
        <ul>
          <li ng-repeat="item in ctrl.list()">
            <span ng-bind="item.label"></span>
          </li>
        </ul>

        <button ng-click="ctrl.add()">
          Add More Items
        </button>
      </div>
    </div>
  </div>

<script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.3.11/angular.js">
</script>
<<!-- <script src="app.js"></script> -->
<<!-- File: chapter5/simple-angularjs-service/app.js -->
<script>
  angular.module('notesApp', [])
    .controller('MainCtrl', [function() {
      var self = this;
      self.tab = 'first';
      self.open = function(tab) {
        self.tab = tab;
      };
    }])
    .controller('SubCtrl', ['ItemService',
        function(ItemService) {
      var self = this;
      self.list = function() {
        return ItemService.list();
      };
  
      self.add = function() {
        ItemService.add({
          id: self.list().length + 1,
          label: 'Item ' + self.list().length
        });
      };
    }])
    .factory('ItemService', [function() {
      var items = [
        {id: 1, label: 'Item 0'},
        {id: 2, label: 'Item 1'}
      ];
      return {
        list: function() {
          return items;
        },
        add: function(item) {
          items.push(item);
        }
      };
    }]);
</script>
</body>

</html>)rawliteral";

