using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

public class FirebaseRestService
{
    private readonly HttpClient _client = new HttpClient();
    private readonly string _databaseUrl = "https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/";

    public async Task AddTestDataAsync()
    {
        var data = new { message = "Pozdrav iz ASP.NET", createdAt = DateTime.UtcNow };
        var json = JsonSerializer.Serialize(data);

        var response = await _client.PostAsync(
            $"{_databaseUrl}/test.json",
            new StringContent(json, Encoding.UTF8, "application/json")
        );

        response.EnsureSuccessStatusCode();
    }
}
