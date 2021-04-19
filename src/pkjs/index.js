// We use the fake 'PBL' symbol as default
var symbol = 0;
var qDogePrice = 'https://chain.so/api/v2/get_price/DOGE/USD';
var qDogeBalance = 'https://dogechain.info/api/v1/address/balance/';
var qCurrency = 'https://v6.exchangerate-api.com/v6/e65e3344d67e23f7e847a141/pair/USD/';
var qDogeBalanceParam = '';
var qCurrencyParam = '';
var price = 0;
var amountUSD = 0;
var balance = 0;

function queryHttp(url, callback) {
  var req = new XMLHttpRequest();

  req.open('GET', url, false);
  req.onload = function (e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        var response = JSON.parse(req.responseText);
        if (response) {
          callback(response);
        }
      } else {
        console.log('Request Error:' + url);
      }
    }
  };
  req.send(null);
}

function getBalance(priceR) {
  if ("" == priceR) return;
  price = priceR['data']['prices'][0]['price']
  console.log("price=" + price);
  queryHttp(qDogeBalance + qDogeBalanceParam, getCurrency);
}

function getPrice() {
  queryHttp(qDogePrice, getBalance);
}

function getCurrency(balanceR) {
  if ("" == balanceR) return;
  amountUSD = price * balanceR['balance']
  console.log("amountUSD=" + amountUSD);
  queryHttp(qCurrency + qCurrencyParam, pushAmount);
}

function pushAmount(currencyR) {
  if ("" == currencyR) return;
  var amount = amountUSD * currencyR['conversion_rate']
  console.log("amount=" + amount.toFixed(2));
  localStorage.setItem('doge-amount', amount.toFixed(2));
  Pebble.sendAppMessage({ '0': amount.toFixed(2) });
}

Pebble.addEventListener('ready', function (e) {
  var amount = localStorage.getItem('doge-amount');
  console.log("amount=" + amount);
  if (amount) {
    Pebble.sendAppMessage({ '0': amount });
  }
});

Pebble.addEventListener('appmessage', function (e) {
  qDogeBalanceParam = e.payload[0];
  qCurrencyParam = e.payload[1];
  getPrice();
  console.log('amount request start.');
});

