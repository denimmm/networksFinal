const baseUrl = "http://localhost:23500";

async function connect() {
    const ip = document.getElementById("ip").value;
    const port = document.getElementById("port").value;

    const res = await fetch(`${baseUrl}/connect/${ip}/${port}`);
    const text = await res.text();
    document.getElementById("response").innerText = text;
}

async function sendTelecommand() {
const cmdJson = document.getElementById("telecommand").value;
const res = await fetch(`${baseUrl}/telecommand/`, {
        method: "PUT",
        headers: {
            "Content-Type": "application/json"
        },
        body: cmdJson
    });
const text = await res.text();
document.getElementById("response").innerText = text;
}

async function getTelemetry() {
const res = await fetch(`${baseUrl}/telemetry_request/`);
const text = await res.text();
document.getElementById("response").innerText = text;
}
