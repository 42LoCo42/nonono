<script lang="ts">
	export let counter: number;

	async function reset() {
		await fetch('/api/counter', {
			method: 'DELETE',
		});

		counter = 0;
	}
</script>

<button onclick={reset}>Reset</button>
