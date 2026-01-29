using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using backend.Models;

namespace backend.Services
{
    public class FirebaseService
    {
        private readonly HttpClient _client;
        private readonly string _databaseUrl = "https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/";

        public FirebaseService()
        {
            _client = new HttpClient();
        }

        public async Task<SensorData?> GetSensorDataAsync()
        {
            try
            {
                var response = await _client.GetAsync($"{_databaseUrl}.json");
                response.EnsureSuccessStatusCode();

                var json = await response.Content.ReadAsStringAsync();
                var data = JsonSerializer.Deserialize<SensorData>(json, new JsonSerializerOptions
                {
                    PropertyNameCaseInsensitive = true
                });

                return data;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error fetching sensor data: {ex.Message}");
                return null;
            }
        }

        public async Task<bool> UpdateGasLimitAsync(int gasLimit)
        {
            try
            {
                var data = new { gasLimit = gasLimit };
                var json = JsonSerializer.Serialize(data);

                var response = await _client.PatchAsync(
                    $"{_databaseUrl}.json",
                    new StringContent(json, Encoding.UTF8, "application/json")
                );

                response.EnsureSuccessStatusCode();
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error updating gas limit: {ex.Message}");
                return false;
            }
        }
    }
}
