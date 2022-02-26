var Xray = require('x-ray')
var x = Xray()

x('https://www.youtube.com/channel/UCFwMITSkc1Fms6PoJoh1OUQ/videos', [
    {
        overlayStyle: 'span'
    }
])
  .then(function(res) {
      console.log(res);
  });