const express = require('express')
const app = express()
const port = 3002
const axios = require('axios'); // Axios pour faire des requêtes HTTP
const apiKey = 'c4a6546ceda64b05ba7134102242901';
const cities = ['Paris', 'London', 'Lisbon', 'Rome', 'Moscow', 'New York'];
let weatherData = {};

app.get('/', (req, res) => {
  res.send('Hello World!')
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})
// Fonction pour récupérer les données météo
const fetchWeatherData = async () => {
  for (let city of cities) {
    const url = `http://api.weatherapi.com/v1/current.json?key=${apiKey}&q=${city}&aqi=no`;
    try {
      const response = await axios.get(url);
      weatherData[city] = response.data;
    } catch (error) {
      console.error(`Erreur lors de la récupération des données pour ${city}:`, error);
    }
  }
};

fetchWeatherData(); // Première récupération au démarrage

// Route pour obtenir les données météo d'une ville spécifique
app.get('/weather/:city', (req, res) => {
  //fetchWeatherData();
  const city = req.params.city;
  const data = weatherData[city];
  if (data) {
    res.json(data);
  } else {
    res.status(404).send('Données non trouvées pour la ville demandée');
  }
});
