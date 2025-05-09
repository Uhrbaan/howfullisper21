function openModal(id) {
  const title = {
    lab1: "Info Lab 1",
    lab2: "Info Lab 2",
    biblio: "Bibliothèque"
  };

  const imagePath = {
    lab1: "/static/img/lab1.png",
    lab2: "/static/img/lab2.png",
    biblio: "/static/img/biblio.png"
  };

  document.getElementById('modal-title').innerText = title[id];
  document.getElementById('modal-image').src = imagePath[id];
  document.getElementById('modal').style.display = 'flex';
}

function closeModal() {
  document.getElementById('modal').style.display = 'none';
}
