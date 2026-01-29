using Microsoft.AspNetCore.Mvc;

[ApiController]
[Route("[controller]")]
public class TestController : ControllerBase
{
    private readonly FirebaseRestService _firebaseService = new FirebaseRestService();

    [HttpGet("add")]
    public async Task<IActionResult> AddTestData()
    {
        await _firebaseService.AddTestDataAsync();
        return Ok("Podaci poslati u Firebase!");
    }
}
